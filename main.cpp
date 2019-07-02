#include <igl/readOBJ.h>
#include <igl/cotmatrix.h>
#include <igl/eigs.h>
#include <igl/massmatrix.h>
#include <Eigen/SparseExtra>
#include "Viewer.h"
#include "HE.h"
#include "SteepLine.h"
#include "MSComplex.h"

using namespace std;

int EIG_NUM = 8;

int main(int argc, char *argv[])
{
	Eigen::MatrixXd tcs;
	Eigen::MatrixXi ftcs;

	auto vertices_ptr = std::make_shared<Eigen::MatrixXd>();
	auto vns_ptr = std::make_shared<Eigen::MatrixXd>();
	auto faces_ptr = std::make_shared<Eigen::MatrixXi>();
	auto fns_ptr = std::make_shared<Eigen::MatrixXi>();

	glutInit(&argc, argv);
	// //Load a mesh in OFF format
	igl::readOBJ("../models/sphere1.obj", *vertices_ptr, tcs, *vns_ptr, *faces_ptr, ftcs, *fns_ptr);

	// construct Half Edges structure
	std::shared_ptr<HE> he = std::make_shared<HE>(*faces_ptr, vertices_ptr->rows());
	he->buildHEs();
	//std::shared_ptr<std::vector<int>> neighbors = he.getNeighbors(55);

	Eigen::SparseMatrix<double> L, M;
	igl::cotmatrix(*vertices_ptr, *faces_ptr, L);
	L = (-L).eval();

	igl::massmatrix(*vertices_ptr, *faces_ptr, igl::MASSMATRIX_TYPE_DEFAULT, M);

	Eigen::MatrixXd U;
	Eigen::VectorXd D;
	//igl::eigs(L, M, 5, igl::EIGS_TYPE_SM, U, D);
	if (!igl::eigs(L, M, EIG_NUM, igl::EIGS_TYPE_SM, U, D))
	{
		cout << "failed." << endl;
		exit(1);
	}

	U = ((U.array() - U.minCoeff()) / (U.maxCoeff() - U.minCoeff())).eval();

	auto eigenVec_ptr = std::make_shared<Eigen::VectorXd>(U.col(0));

	std::shared_ptr<SteepLine> SL = std::make_shared<SteepLine>(*eigenVec_ptr, *he);
	SL->getVerticesType();
	std::shared_ptr<std::vector<std::vector<int>>> steepLines = SL->getSteepLines();

	MSComplex::setSL(SL);
	MSComplex::setHE(he);
	MSComplex::buildAdjMatrix();
	std::shared_ptr<std::vector<MSComplex::ms_region_t>> ms_regions = MSComplex::buildMSComplex();

	int N = vertices_ptr->size();
	std::shared_ptr<std::vector<int>> partitions = std::make_shared<std::vector<int>>(N);
	for (int i = 0; i < ms_regions->size(); i++) {
		std::vector<int> &region_verts = ms_regions->at(i).region_verts;
		for (int j = 0; j < region_verts.size(); j++) {
			partitions->at(region_verts.at(j)) = i;
		}
	}

	std::cout << U.col(4) << std::endl;
	viewer::setMeshInfo(vertices_ptr, faces_ptr, vns_ptr, fns_ptr, eigenVec_ptr);
	viewer::setDrawMode(viewer::DrawMode::PSEUDO_COLOR);
	viewer::setPartition(partitions);
	viewer::prepareMesh();
	viewer::passLines(steepLines);
	viewer::view();
	cout << "hi" << endl;
}
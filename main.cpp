#include <igl/readOBJ.h>
#include <igl/cotmatrix.h>
#include <igl/eigs.h>
#include <igl/massmatrix.h>
#include <Eigen/SparseExtra>
#include <iostream>
#include "Viewer.h"
#include "HE.h"
#include "SteepLine.h"
#include "MSComplex.h"
#include "Relaxation.h"

using namespace std;

int EIG_NUM = 10;
bool readmode = true;

int main(int argc, char *argv[])
{
	std::shared_ptr<Eigen::VectorXd> eigenVec_ptr;

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

	//std::cout << "L non zero entries: " << std::endl;
	//for (int i = 0; i < L.rows(); i++) {
	//	for (int j = 0; j < L.cols(); j++) {
	//		if (L.coeff(i, j) != 0) {
	//			std::cout << L.coeff(i, j) << std::endl;
	//		}
	//	}
	//}

	if (!readmode) {
		Eigen::MatrixXd U;
		Eigen::VectorXd D;
		//igl::eigs(L, M, 5, igl::EIGS_TYPE_SM, U, D);
		if (!igl::eigs(L, M, EIG_NUM, igl::EIGS_TYPE_SM, U, D))
		{
			std::cout << "failed." << endl;
			exit(1);
		}

		U = ((U.array() - U.minCoeff()) / (U.maxCoeff() - U.minCoeff())).eval();

		eigenVec_ptr = std::make_shared<Eigen::VectorXd>(U.col(0));
		std::cout << "D: " << D << std::endl;

		// write eigen vector to file
		ofstream myfile;
		myfile.open("eigen_vec.txt", ios::out);
		if (myfile.is_open()) {
			for (int i = 0; i < eigenVec_ptr->rows(); i++) {
				myfile << (*eigenVec_ptr)(i) << "\n";
				//std::cout << (*eigenVec_ptr)(i) << endl;
			}
			myfile.close();
		}
	}
	else {
		eigenVec_ptr = std::make_shared<Eigen::VectorXd>(vertices_ptr->rows());
		// read eigen vector from file
		ifstream myfile;
		string line;
		myfile.open("eigen_vec_10.txt");
		if (myfile.is_open()) {
			int line_num = 0;
			while (getline(myfile, line))
			{
				float num = std::stof(line);
				(*eigenVec_ptr)(line_num) = num;
				line_num++;
			}
			myfile.close();
		}
	}

	std::shared_ptr<SteepLine> SL = std::make_shared<SteepLine>(*eigenVec_ptr, *he);
	SL->getVerticesType();
	std::shared_ptr<std::vector<std::vector<int>>> steepLines = SL->getSteepLines();

	MSComplex::setSL(SL);
	MSComplex::setHE(he);
	MSComplex::buildAdjMatrix();
	std::shared_ptr<std::vector<MSComplex::ms_region_t>> ms_regions = MSComplex::buildMSComplex();
	MSComplex::parametrize();

	// set partitions
	int N = vertices_ptr->size();
	std::shared_ptr<std::vector<int>> partitions = MSComplex::getPartitions();

	// Apply relaxation
	Relaxation::setInfos(he, MSComplex::getAdjIndex(), partitions, std::make_shared<decltype(L)>(L), MSComplex::getMSRegions());
	Relaxation::setMtxIndices();
	Relaxation::solveU();
	viewer::setMeshInfo(vertices_ptr, faces_ptr, vns_ptr, fns_ptr, eigenVec_ptr);
	viewer::setDrawMode(viewer::DrawMode::PSEUDO_COLOR);
	viewer::setPartition(partitions);
	viewer::prepareMesh();
	viewer::passLines(steepLines);
	viewer::view();
	cout << "hi" << endl;
}
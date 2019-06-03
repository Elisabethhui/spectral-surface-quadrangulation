#include <igl/readOBJ.h>
#include <igl/cotmatrix.h>
#include <igl/eigs.h>
#include <igl/massmatrix.h>
#include <Eigen/SparseExtra>
#include "Viewer.h"
#include "HE.h"

using namespace std;

int EIG_NUM = 5;

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
	igl::readOBJ("../models/sphere.obj", *vertices_ptr, tcs, *vns_ptr, *faces_ptr, ftcs, *fns_ptr);

	// construct Half Edges structure
	HE he(*faces_ptr, vertices_ptr->rows());
	he.buildHEs();
	std::shared_ptr<std::vector<int>> neighbors = he.getNeighbors(55);

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
	}

	U = ((U.array() - U.minCoeff()) / (U.maxCoeff() - U.minCoeff())).eval();

	auto eigenVec_ptr = std::make_shared<Eigen::VectorXd>(U.col(0));

	std::cout << U.col(4) << std::endl;
	//Viewer viewer(vertices_ptr, faces_ptr);
	//Viewer viewer(vertices_ptr, faces_ptr, vns_ptr, fns_ptr);
	Viewer viewer(vertices_ptr, faces_ptr, vns_ptr, fns_ptr, eigenVec_ptr);
	viewer.view();
	cout << "hi" << endl;
}
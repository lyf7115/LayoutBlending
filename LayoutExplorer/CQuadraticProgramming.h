#pragma once

#include "CLayoutTree.h"
#include "CCompoundNode.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <queue>
#include <unordered_map>
#include <numeric>

bool CompoundCompareY(CompoundNode* a, CompoundNode* b);
bool CompoundCompareX(CompoundNode* a, CompoundNode* b);

void ConstructFunction(CompoundNode* node, double& slider_value);

Eigen::VectorXd SolveFunction(Eigen::MatrixXd H, Eigen::MatrixXd A, Eigen::VectorXd b, Eigen::VectorXd c);

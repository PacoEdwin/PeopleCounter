// project includes
#include "Object.h"

// std includes
#include <limits>
#include <random>
#include <iostream>

// cv includes
#include <opencv2/core/core.hpp>

Object::Object() 
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(0, std::numeric_limits<int>::max());

	id_ = dis(gen);
	dis = std::uniform_int_distribution<>(0, 255);
	color_ = cv::Scalar(dis(gen), dis(gen), dis(gen));
}

Object::Object(const cv::Point& value) :
	location_(value)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(0, std::numeric_limits<int>::max());

	id_ = dis(gen);
	dis = std::uniform_int_distribution<>(0, 255);
	color_ = cv::Scalar(dis(gen), dis(gen), dis(gen));
}

Object::Object(const Object& rhs):
	id_(rhs.id_),
	color_(rhs.color_),
	location_(rhs.location_){}

int Object::id() const
{
	return id_;
}
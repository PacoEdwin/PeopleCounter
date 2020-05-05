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

	m_id = dis(gen);
	dis = std::uniform_int_distribution<>(0, 255);
	m_color = cv::Scalar(dis(gen), dis(gen), dis(gen));
}

Object::Object(const cv::Point& value) :
	m_location(value)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(0, std::numeric_limits<int>::max());

	m_id = dis(gen);
	dis = std::uniform_int_distribution<>(0, 255);
	m_color = cv::Scalar(dis(gen), dis(gen), dis(gen));
}

Object::Object(const Object& rhs):
	m_id(rhs.m_id),
	m_color(rhs.m_color),
	m_location(rhs.m_location),
	m_name(rhs.m_name) {}

int Object::id() const
{
	return m_id;
}

uint Object::name() const
{
	return m_name;
}

void Object::setName(uint name)
{
	m_name = name;
}
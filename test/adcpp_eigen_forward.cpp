/* adcpp_eigen_forward.cpp
 *
 *  Created on: 21 Aug 2019
 *      Author: Fabian Meyer
 */

#include <catch2/catch.hpp>
#include <adcpp_eigen.h>
#include <Eigen/Geometry>
#include <iostream>

using namespace adcpp;

TEST_CASE("Eigen forward algorithmic differentiation")
{
    double eps = 1e-6;

    SECTION("exp")
    {
        fwd::Vector2d x;
        x << fwd::Double(3,1), fwd::Double(2,0);

        Eigen::Vector2d valExp;
        valExp <<
            std::exp(x(0).value()),std::exp(x(1).value());
        Eigen::Matrix2d jacExp;
        jacExp << std::exp(x(0).value()), 0,
            0, std::exp(x(1).value());

        fwd::Vector2d fx = x.array().exp();

        x << fwd::Double(3,0), fwd::Double(2,1);
        fwd::Vector2d fy = x.array().exp();

        REQUIRE(Approx(valExp(0)).margin(eps) == fx(0).value());
        REQUIRE(Approx(valExp(1)).margin(eps) == fx(1).value());
        REQUIRE(Approx(valExp(0)).margin(eps) == fy(0).value());
        REQUIRE(Approx(valExp(1)).margin(eps) == fy(1).value());

        REQUIRE(Approx(jacExp(0, 0)).margin(eps) == fx(0).gradient());
        REQUIRE(Approx(jacExp(1, 0)).margin(eps) == fx(1).gradient());
        REQUIRE(Approx(jacExp(0, 1)).margin(eps) == fy(0).gradient());
        REQUIRE(Approx(jacExp(1, 1)).margin(eps) == fy(1).gradient());
    }

    SECTION("singular value decomposition")
    {
        fwd::Matrix4d A;
        A << fwd::Double(2, 1), 3, 11, 5,
            1, 1, 5, 2,
            2, 1, -3, 2,
            1, 1, -3, 4;
        fwd::Vector4d b;
        b << 2, 1, -3, -3;

        fwd::Vector4d resultAct;
        Eigen::Vector4d valExp;
        Eigen::Vector4d gradExp;
        valExp << -0.5, -0.1875, 0.4375, -0.25;
        gradExp << -0.205283, 0.687338, -0.0722404, -0.117474;

        Eigen::JacobiSVD<fwd::Matrix4d, Eigen::FullPivHouseholderQRPreconditioner>
            solver(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
        resultAct = solver.solve(b);

        REQUIRE(Approx(valExp(0)).margin(eps) == resultAct(0).value());
        REQUIRE(Approx(valExp(1)).margin(eps) == resultAct(1).value());
        REQUIRE(Approx(valExp(2)).margin(eps) == resultAct(2).value());
        REQUIRE(Approx(valExp(3)).margin(eps) == resultAct(3).value());

        REQUIRE(Approx(gradExp(0)).margin(eps) == resultAct(0).gradient());
        REQUIRE(Approx(gradExp(1)).margin(eps) == resultAct(1).gradient());
        REQUIRE(Approx(gradExp(2)).margin(eps) == resultAct(2).gradient());
        REQUIRE(Approx(gradExp(3)).margin(eps) == resultAct(3).gradient());
    }

    SECTION("multiple outputs")
    {
        fwd::Vector2d x;
        x << fwd::Double(3,1), fwd::Double(2,0);

        fwd::Matrix2d c;
        c << fwd::Double(2.1), fwd::Double(3.4),
            fwd::Double(1.6), fwd::Double(2.3);

        Eigen::Vector2d valExp;
        valExp <<
            x(0).value() * c(0, 0).value() +  x(1).value() * c(0, 1).value(),
            x(0).value() * c(1, 0).value() +  x(1).value() * c(1, 1).value();
        Eigen::Matrix2d jacExp;
        jacExp << c(0, 0).value(), c(0, 1).value(),
            c(1, 0).value(), c(1, 1).value();

        fwd::Vector2d fx = c * x;

        x << fwd::Double(3,0), fwd::Double(2,1);
        fwd::Vector2d fy = c * x;

        REQUIRE(Approx(valExp(0)).margin(eps) == fx(0).value());
        REQUIRE(Approx(valExp(1)).margin(eps) == fx(1).value());
        REQUIRE(Approx(valExp(0)).margin(eps) == fy(0).value());
        REQUIRE(Approx(valExp(1)).margin(eps) == fy(1).value());

        REQUIRE(Approx(jacExp(0, 0)).margin(eps) == fx(0).gradient());
        REQUIRE(Approx(jacExp(1, 0)).margin(eps) == fx(1).gradient());
        REQUIRE(Approx(jacExp(0, 1)).margin(eps) == fy(0).gradient());
        REQUIRE(Approx(jacExp(1, 1)).margin(eps) == fy(1).gradient());
    }
}

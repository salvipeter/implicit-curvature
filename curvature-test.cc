#include <dc.hh>

#include "implicit-curvatures.hh"

using namespace Geometry;

int main(int argc, char **argv) {
  // Torus centered at the origin, symmetric about the z axis
  double R = 5.0, r = 2.0;
  double theta = 0.765, phi = 0.435; // "random" point on the torus
  // double theta = M_PI / 2, phi = 0;  // top
  // double theta = 0, phi = 0;         // outside
  // double theta = M_PI, phi = 0;      // inside
  Point3D q((R + r * cos(theta)) * cos(phi),
            (R + r * cos(theta)) * sin(phi),
            r * sin(theta));

  // Evaluation
  auto f = [=](const Point3D &p) {
    double d = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r;
    return 4 * R * R * (p[0] * p[0] + p[1] * p[1]) - d * d;
  };

  // Write surface
  auto dcf = [&](const DualContouring::Point3D &p) {
    return f({ p[0], p[1], p[2] });
  };
  double bb = (R + r) * 1.2;
  size_t res = 50;
  auto mesh = DualContouring::isosurface(dcf, 0.0,
                                         { { { -bb, -bb, -bb }, { bb, bb, bb } } },
                                         { res, res, res });
  mesh.writeOBJ("torus.obj");

  // Derivatives
  auto fx = [=](const Point3D &p) {
    return -4.0 * p[0] * (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r)
      + 8 * R * R * p[0];
  };
  auto fy = [=](const Point3D &p) {
    return -4.0 * p[1] * (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r)
      + 8 * R * R * p[1];
  };
  auto fz = [=](const Point3D &p) {
    return -4.0 * p[2] * (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r);
  };
  auto fxx = [=](const Point3D &p) {
    return -4.0 * (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r)
      - 8 * (p[0] * p[0] - R * R);
  };
  auto fyy = [=](const Point3D &p) {
    return -4.0 * (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r)
      - 8 * (p[1] * p[1] - R * R);
  };
  auto fzz = [=](const Point3D &p) {
    return -4.0 * (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + R * R - r * r)
      - 8 * p[2] * p[2];
  };
  auto fxy = [=](const Point3D &p) {
    return -8 * p[0] * p[1];
  };
  auto fxz = [=](const Point3D &p) {
    return -8 * p[0] * p[2];
  };
  auto fyz = [=](const Point3D &p) {
    return -8 * p[1] * p[2];
  };

  // Gradient & Hessian
  auto grad = [&](const Point3D &p) {
    return Vector3D(fx(p), fy(p), fz(p));
  };
  auto hess = [&](const Point3D &p) {
    double xy = fxy(p), xz = fxz(p), yz = fyz(p);
    double v[] = { fxx(p), xy, xz, xy, fyy(p), yz, xz, yz, fzz(p) };
    return Matrix3x3(v);
  };

  // Tests
  auto g = grad(q);
  auto h = hess(q);
  std::cout << "Gradient: " << g << std::endl;
  std::cout << "Hessian:" << std::endl;
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j)
      std::cout << '\t' << h(i,j);
    std::cout << std::endl;
  }
  std::cout << "Sample point: " << q << std::endl;
  std::cout << "Function value: "
            << f(q) << std::endl;
  std::cout << "Normal vector: " << g.normalized() << std::endl;
  std::cout << "Mean curvature: "
            << ImplicitCurvature::mean(g, h) << std::endl;
  std::cout << "Gaussian curvature: "
            << ImplicitCurvature::gaussian(g, h) << std::endl;
  auto [kmin, kmax] = ImplicitCurvature::principal(g, h);
  std::cout << "Principal curvatures: "
            << kmin << ", " << kmax << std::endl;
  auto [dmin, dmax] = ImplicitCurvature::directions(g, h);
  std::cout << "Principal directions: "
            << dmin << " / " << dmax << std::endl;

  return 0;
}

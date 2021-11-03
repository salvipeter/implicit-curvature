all: curvature-test

FLAGS=-Wall -pedantic -std=c++17
INCLUDES=-I../libgeom -I../dual-contouring
LIBS=-L../libgeom/release -lgeom -L../dual-contouring/build -ldualcontour

curvature-test: curvature-test.cc
	$(CXX) $(FLAGS) $(INCLUDES) -o $@ $< $(LIBS)

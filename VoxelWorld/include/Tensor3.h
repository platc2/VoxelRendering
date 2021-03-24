
template <class T, int dim1, int dim2, int dim3> class Tensor3 {
    T data[dim1][dim2][dim3];

public:
    const T& operator()(const int x, const int y, const int z) const {
        return this->data[x][y][z];
    }

    T& operator()(const int x, const int y, const int z) {
        return this->data[x][y][z];
    }
};

inline v2 V2(double x, double y)
{
    v2 vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

inline v3 V3(double x, double y, double z)
{
    v3 vector;

    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

inline double math_magnitude(v2 vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}

inline double math_magnitude(v3 vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

inline bool is_null_vector(v2 vector)
{
    if(abs(vector.x) > 0.00001 || abs(vector.y) > 0.00001)
    {
        return false;
    }

    return true;
}

inline bool is_null_vector(v3 vector)
{
    if(abs(vector.x) > 0.00001 || abs(vector.y) > 0.00001 || abs(vector.z) > 0.00001)
    {
        return false;
    }

    return true;
}

inline v2 math_normalize(v2 vector)
{
    assert(abs(vector.x) > 0.00001 || abs(vector.y) > 0.00001);

    double magnitude = math_magnitude(vector);

    vector.x /= magnitude;
    vector.y /= magnitude;

    return vector;
}

inline v3 math_normalize(v3 vector)
{
    assert(abs(vector.x) > 0.00001 || abs(vector.y) > 0.00001 || abs(vector.z) > 0.00001);

    double magnitude = math_magnitude(vector);

    vector.x /= magnitude;
    vector.y /= magnitude;
    vector.z /= magnitude;

    return vector;
}

inline v2 rotate_vector(v2 v, double angle)
{
    double new_x = v.x * cos(angle) - v.y * sin(angle);
    double new_y = v.x * sin(angle) + v.y * cos(angle);

    v.x = new_x;
    v.y = new_y;

    return v;
}

inline v2 operator-(v2 a, v2 b)
{
    return V2(a.x - b.x, a.y - b.y);
}

inline v3 operator-(v3 a, v3 b)
{
    return V3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline v2 & operator-=(v2 &a, v2 b)
{
    a.x -= b.x;
    a.y -= b.y;

    return a;
}

inline v3 & operator-=(v3 &a, v3 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;

    return a;
}

inline v2 operator-(v2 a)
{
    return V2(-a.x, -a.y);
}

inline v3 operator-(v3 a)
{
    return V3(-a.x, -a.y, -a.z);
}

inline v2 operator+(v2 a, v2 b)
{
    return V2(a.x + b.x, a.y + b.y);
}

inline v3 operator+(v3 a, v3 b)
{
    return V3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline v2 & operator+=(v2 &a, v2 b)
{
    a.x += b.x;
    a.y += b.y;

    return a;
}

inline v3 & operator+=(v3 &a, v3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;

    return a;
}

inline v2 operator*(v2 a, double b)
{
    return V2(a.x * b, a.y * b);
}

inline v3 operator*(v3 a, double b)
{
    return V3(a.x * b, a.y * b, a.z * b);
}

inline v2 operator/(v2 a, double b)
{
    return V2(a.x / b, a.y / b);
}

inline v3 operator/(v3 a, double b)
{
    return V3(a.x / b, a.y / b, a.z / b);
}

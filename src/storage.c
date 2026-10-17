#include "academic_os.h"
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #define MAKE_DIR(path) _mkdir(path)
#else
    #define MAKE_DIR(path) mkdir(path, 0755)
#endif

static AppState g_state;

AppState* get_app_state(void) {
    return &g_state;
}

const char* class_type_to_string(ClassType type) {
    switch (type) {
        case CLASS_TYPE_THEORY:   return "Theory";
        case CLASS_TYPE_LAB:      return "Lab";
        case CLASS_TYPE_TUTORIAL: return "Tutorial";
        case CLASS_TYPE_SEMINAR:  return "Seminar";
        default:                  return "Class";
    }
}

const char* exam_type_to_string(ExamType type) {
    switch (type) {
        case EXAM_TYPE_QUIZ:         return "Quiz";
        case EXAM_TYPE_CT:           return "Class Test";
        case EXAM_TYPE_MIDTERM:      return "Midterm";
        case EXAM_TYPE_FINAL:        return "Final";
        case EXAM_TYPE_LAB_EXAM:     return "Lab Exam";
        case EXAM_TYPE_VIVA:         return "Viva";
        case EXAM_TYPE_ASSIGNMENT:   return "Assignment";
        case EXAM_TYPE_PRESENTATION: return "Presentation";
        default:                     return "Assessment";
    }
}

const char* topic_status_to_string(TopicStatus status) {
    switch (status) {
        case TOPIC_STATUS_COMPLETED:   return "COMPLETED";
        case TOPIC_STATUS_IN_PROGRESS: return "IN PROGRESS";
        case TOPIC_STATUS_PENDING:     return "PENDING";
        default:                       return "PENDING";
    }
}

const char* day_to_string(int day) {
    static const char* days[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };
    if (day >= 0 && day < 7) return days[day];
    return "Unknown";
}

const char* day_to_short_string(int day) {
    static const char* sdays[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    if (day >= 0 && day < 7) return sdays[day];
    return "???";
}

static void ensure_data_directory(void) {
    MAKE_DIR("data");
}

bool storage_save_courses(void) {
    ensure_data_directory();
    FILE* fp = fopen("data/courses.dat", "wb");
    if (!fp) return false;
    fwrite(&g_state.course_count, sizeof(int), 1, fp);
    if (g_state.course_count > 0) {
        fwrite(g_state.courses, sizeof(Course), g_state.course_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_load_courses(void) {
    FILE* fp = fopen("data/courses.dat", "rb");
    if (!fp) return false;
    if (fread(&g_state.course_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (g_state.course_count > MAX_COURSES) g_state.course_count = MAX_COURSES;
    if (g_state.course_count > 0) {
        fread(g_state.courses, sizeof(Course), g_state.course_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_save_routine(void) {
    ensure_data_directory();
    FILE* fp = fopen("data/routine.dat", "wb");
    if (!fp) return false;
    fwrite(&g_state.class_count, sizeof(int), 1, fp);
    if (g_state.class_count > 0) {
        fwrite(g_state.classes, sizeof(RoutineClass), g_state.class_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_load_routine(void) {
    FILE* fp = fopen("data/routine.dat", "rb");
    if (!fp) return false;
    if (fread(&g_state.class_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (g_state.class_count > MAX_CLASSES) g_state.class_count = MAX_CLASSES;
    if (g_state.class_count > 0) {
        fread(g_state.classes, sizeof(RoutineClass), g_state.class_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_save_exams(void) {
    ensure_data_directory();
    FILE* fp = fopen("data/exams.dat", "wb");
    if (!fp) return false;
    fwrite(&g_state.exam_count, sizeof(int), 1, fp);
    if (g_state.exam_count > 0) {
        fwrite(g_state.exams, sizeof(Exam), g_state.exam_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_load_exams(void) {
    FILE* fp = fopen("data/exams.dat", "rb");
    if (!fp) return false;
    if (fread(&g_state.exam_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (g_state.exam_count > MAX_EXAMS) g_state.exam_count = MAX_EXAMS;
    if (g_state.exam_count > 0) {
        fread(g_state.exams, sizeof(Exam), g_state.exam_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_save_syllabus(void) {
    ensure_data_directory();
    FILE* fp = fopen("data/syllabus.dat", "wb");
    if (!fp) return false;
    fwrite(&g_state.syllabus_count, sizeof(int), 1, fp);
    if (g_state.syllabus_count > 0) {
        fwrite(g_state.syllabus, sizeof(SyllabusTopic), g_state.syllabus_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_load_syllabus(void) {
    FILE* fp = fopen("data/syllabus.dat", "rb");
    if (!fp) return false;
    if (fread(&g_state.syllabus_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (g_state.syllabus_count > MAX_TOPICS) g_state.syllabus_count = MAX_TOPICS;
    if (g_state.syllabus_count > 0) {
        fread(g_state.syllabus, sizeof(SyllabusTopic), g_state.syllabus_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_save_grades(void) {
    ensure_data_directory();
    FILE* fp = fopen("data/grades.dat", "wb");
    if (!fp) return false;
    fwrite(&g_state.grade_count, sizeof(int), 1, fp);
    if (g_state.grade_count > 0) {
        fwrite(g_state.grades, sizeof(GradeRecord), g_state.grade_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_load_grades(void) {
    FILE* fp = fopen("data/grades.dat", "rb");
    if (!fp) return false;
    if (fread(&g_state.grade_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (g_state.grade_count > MAX_GRADES) g_state.grade_count = MAX_GRADES;
    if (g_state.grade_count > 0) {
        fread(g_state.grades, sizeof(GradeRecord), g_state.grade_count, fp);
    }
    fclose(fp);
    return true;
}

bool storage_save_settings(void) {
    ensure_data_directory();
    FILE* fp = fopen("data/settings.dat", "wb");
    if (!fp) return false;
    fwrite(&g_state.settings, sizeof(AppSettings), 1, fp);
    fclose(fp);
    return true;
}

bool storage_load_settings(void) {
    FILE* fp = fopen("data/settings.dat", "rb");
    if (!fp) return false;
    if (fread(&g_state.settings, sizeof(AppSettings), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    fclose(fp);
    return true;
}

bool storage_save_all(void) {
    bool ok = true;
    ok &= storage_save_courses();
    ok &= storage_save_routine();
    ok &= storage_save_exams();
    ok &= storage_save_syllabus();
    ok &= storage_save_grades();
    ok &= storage_save_settings();
    return ok;
}

/* ========================================================================= */
/* OFFICIAL MATH 1107: DIFFERENTIAL & INTEGRAL CALCULUS LECTURE PLAN         */
/* Pre-Mid Term (Lec 1-19): Md. Dulal Hossain, Assistant Professor            */
/* Post-Mid Term (Lec 20-39): Mst. Munny Khatun, Lecturer                    */
/* ========================================================================= */
static const SyllabusTopic g_math_topics[39] = {
    /* --- PART 1: PRE-MID TERM (Lectures 01 to 19) --- Teacher: Md. Dulal Hossain --- */
    {
        40, "MATH 1107", 1, "[Pre-Mid] Lec 01: Functions, Limits & Continuity", TOPIC_STATUS_COMPLETED, 1,
        {{"Introduction to functions, limits and continuity", TOPIC_STATUS_COMPLETED, "28/06/2026 (28/06/26) • Domain, standard limits & continuity"}},
        "/* Epsilon-Delta Limit & Continuity */\n// lim_{x->c} f(x) = L  <=>  forall eps > 0, exists delta > 0:\n// 0 < |x - c| < delta ==> |f(x) - L| < eps\n// f is continuous at c if lim_{x->c} f(x) = f(c)"
    },
    {
        41, "MATH 1107", 2, "[Pre-Mid] Lec 02: Continuity & Differentiability", TOPIC_STATUS_COMPLETED, 1,
        {{"Continuity and differentiability", TOPIC_STATUS_COMPLETED, "01/07/2026 (01/07/26) • Left & right limits, differentiability criteria"}},
        "/* Differentiability from First Principles */\n// f'(x) = lim_{h->0} [f(x + h) - f(x)] / h\n// Differentiability implies continuity."
    },
    {
        42, "MATH 1107", 3, "[Pre-Mid] Lec 03: Applications of Derivatives", TOPIC_STATUS_COMPLETED, 1,
        {{"Applications of derivatives", TOPIC_STATUS_COMPLETED, "02/07/2026 (02/07/26) • Rate of change, motion, velocity, acceleration"}},
        "/* Applications of Derivatives */\n// Velocity: v(t) = ds/dt\n// Acceleration: a(t) = dv/dt = d^2s/dt^2\n// Slope of tangent to y = f(x): m = dy/dx"
    },
    {
        43, "MATH 1107", 4, "[Pre-Mid] Lec 04: Successive Differentiation", TOPIC_STATUS_COMPLETED, 1,
        {{"Successive diff of algebraic & transcendental functions", TOPIC_STATUS_COMPLETED, "05/07/2026 (05/07/26) • n-th derivatives of e^(ax), sin(ax+b)"}},
        "/* Standard n-th Derivatives */\n// d^n/dx^n [e^(ax)] = a^n * e^(ax)\n// d^n/dx^n [sin(ax+b)] = a^n * sin(ax + b + n*pi/2)\n// d^n/dx^n [(ax+b)^(-1)] = (-1)^n * n! * a^n / (ax+b)^(n+1)"
    },
    {
        44, "MATH 1107", 5, "[Pre-Mid] Lec 05: Leibnitz's Theorem & Higher Derivatives", TOPIC_STATUS_COMPLETED, 1,
        {{"Leibnitz's theorem and higher-order derivatives", TOPIC_STATUS_COMPLETED, "08/07/2026 (08/07/26) • Product rule for n-th order derivatives"}},
        "/* Leibnitz's Theorem for Product (u*v) */\n// D^n(uv) = sum_{k=0}^{n} C(n,k) * D^{n-k}(u) * D^k(v)\n// = u_n*v + n*u_{n-1}*v_1 + [n(n-1)/2!]*u_{n-2}*v_2 + ... + u*v_n"
    },
    {
        45, "MATH 1107", 6, "[Pre-Mid] Lec 06: Taylor's Theorem: Finite & Infinite", TOPIC_STATUS_COMPLETED, 1,
        {{"Taylor's theorem expansion in finite and infinite forms", TOPIC_STATUS_COMPLETED, "09/07/2026 (09/07/26) • Lagrange & Cauchy remainder forms"}},
        "/* Taylor's Series Expansion about x = a */\n// f(x) = f(a) + f'(a)(x-a) + [f''(a)/2!](x-a)^2 + ... + R_n(x)\n// Lagrange form: R_n(x) = [f^(n)(xi) / n!] * (x - a)^n"
    },
    {
        46, "MATH 1107", 7, "[Pre-Mid] Lec 07: Maclaurin's Theorem & Applications", TOPIC_STATUS_COMPLETED, 1,
        {{"Maclaurin's theorem in finite & infinite forms and apps", TOPIC_STATUS_COMPLETED, "12/07/2026 (12/07/26) • Series expansions for e^x, sin(x), cos(x)"}},
        "/* Standard Maclaurin Expansions (about x = 0) */\n// e^x     = 1 + x + x^2/2! + x^3/3! + ...\n// sin(x)  = x - x^3/3! + x^5/5! - x^7/7! + ...\n// ln(1+x) = x - x^2/2 + x^3/3 - x^4/4 + ..."
    },
    {
        47, "MATH 1107", 8, "[Pre-Mid] Lec 08: Rolle's & Mean Value Theorems", TOPIC_STATUS_COMPLETED, 1,
        {{"Rolle's theorem and Mean Value Theorem", TOPIC_STATUS_COMPLETED, "15/07/2026 (15/07/26) • Rolle's theorem, Lagrange & Cauchy MVT"}},
        "/* Rolle's Theorem & Mean Value Theorem */\n// Rolle: f(a)=f(b) ==> exists c in (a,b): f'(c) = 0\n// Lagrange MVT: exists c in (a,b): f'(c) = [f(b) - f(a)] / (b - a)"
    },
    {
        48, "MATH 1107", 9, "[Pre-Mid] Lec 09: Indeterminate Forms & L'Hospital", TOPIC_STATUS_COMPLETED, 1,
        {{"Indeterminate forms and L'Hospital's Rule", TOPIC_STATUS_COMPLETED, "16/07/2026 (16/07/26) • 0/0, inf/inf, 0*inf, 0^0, 1^inf forms"}},
        "/* L'Hospital's Rule */\n// If lim f(x)/g(x) is 0/0 or inf/inf:\n// lim_{x->c} [f(x) / g(x)] = lim_{x->c} [f'(x) / g'(x)]"
    },
    {
        49, "MATH 1107", 10, "[Pre-Mid] Lec 10: Partial Differentiation & Totals", TOPIC_STATUS_COMPLETED, 1,
        {{"Partial differentiation and total derivatives", TOPIC_STATUS_COMPLETED, "19/07/2026 (19/07/26) • Multi-variable functions, mixed partials"}},
        "/* Partial Derivatives */\n// dz/dx = lim_{h->0} [f(x+h, y) - f(x, y)] / h\n// Mixed Partials: d^2z / (dx dy) = d^2z / (dy dx)"
    },
    {
        50, "MATH 1107", 11, "[Pre-Mid] Lec 11: Partial Derivatives [CT-1]", TOPIC_STATUS_COMPLETED, 1,
        {{"Partial differentiation and total derivatives", TOPIC_STATUS_COMPLETED, "22/07/2026 (22/07/26) ★ CLASS TEST 1 (CT-1) • Total derivatives & chain rule"}},
        "/* Total Differential & Chain Rule */\n// dz = (partial z / partial x) dx + (partial z / partial y) dy\n// dz/dt = (dz/dx)*(dx/dt) + (dz/dy)*(dy/dt)"
    },
    {
        51, "MATH 1107", 12, "[Pre-Mid] Lec 12: Euler's Theorem on Homogeneous", TOPIC_STATUS_COMPLETED, 1,
        {{"Euler's theorem on homogeneous functions", TOPIC_STATUS_COMPLETED, "23/07/2026 (23/07/26) • Degree n relations, x*(dz/dx) + y*(dz/dy) = n*z"}},
        "/* Euler's Theorem for Homogeneous Functions */\n// If f(tx, ty) = t^n * f(x, y), then:\n// x * (df/dx) + y * (df/dy) = n * f(x, y)"
    },
    {
        52, "MATH 1107", 13, "[Pre-Mid] Lec 13: Maxima & Minima of One Variable", TOPIC_STATUS_COMPLETED, 1,
        {{"Maxima and minima of functions of one variable", TOPIC_STATUS_COMPLETED, "26/07/2026 (26/07/26) • Stationary points, 1st & 2nd derivative tests"}},
        "/* Maxima & Minima Criteria */\n// Stationary point: f'(x) = 0\n// If f''(x) < 0 ==> Local Maximum\n// If f''(x) > 0 ==> Local Minimum"
    },
    {
        53, "MATH 1107", 14, "[Pre-Mid] Lec 14: Points of Inflection & Applications", TOPIC_STATUS_COMPLETED, 1,
        {{"Maxima, minima & points of inflection with applications", TOPIC_STATUS_COMPLETED, "29/07/2026 (29/07/26) • Concavity & point of inflection"}},
        "/* Points of Inflection */\n// Concave up: f''(x) > 0 | Concave down: f''(x) < 0\n// Inflection point: f''(x) = 0 with change of concavity sign"
    },
    {
        54, "MATH 1107", 15, "[Pre-Mid] Lec 15: Tangents and Normals to Curves", TOPIC_STATUS_COMPLETED, 1,
        {{"Tangents and normals to curves", TOPIC_STATUS_COMPLETED, "30/07/2026 (30/07/26) • Slope m = dy/dx, tangent & normal line equations"}},
        "/* Tangent and Normal Lines at (x1, y1) */\n// Tangent: Y - y1 = m * (X - x1), where m = [dy/dx]\n// Normal:  Y - y1 = (-1/m) * (X - x1)"
    },
    {
        55, "MATH 1107", 16, "[Pre-Mid] Lec 16: Subtangent & Subnormal Formulas", TOPIC_STATUS_COMPLETED, 1,
        {{"Subtangent and subnormal in Cartesian and polar coordinates", TOPIC_STATUS_COMPLETED, "02/08/2026 (02/08/26) • Cartesian & Polar geometric lengths"}},
        "/* Subtangent & Subnormal */\n// Cartesian: Subtangent = |y / (dy/dx)|, Subnormal = |y * (dy/dx)|\n// Polar: Subtangent = r^2 * (d_theta/dr), Subnormal = dr/d_theta"
    },
    {
        56, "MATH 1107", 17, "[Pre-Mid] Lec 17: Asymptotes of Algebraic Curves", TOPIC_STATUS_COMPLETED, 1,
        {{"Asymptotes of algebraic curves", TOPIC_STATUS_COMPLETED, "05/08/2026 (05/08/26) • Parallel & oblique asymptotes y = mx + c"}},
        "/* Oblique Asymptotes */\n// y = mx + c\n// m = lim_{x->inf} [y / x]\n// c = lim_{x->inf} [y - mx]"
    },
    {
        57, "MATH 1107", 18, "[Pre-Mid] Lec 18: Curvature & Radius of Curvature", TOPIC_STATUS_COMPLETED, 1,
        {{"Curvature and radius of curvature", TOPIC_STATUS_COMPLETED, "06/08/2026 (06/08/26) • Radius rho = [1+(y')^2]^(3/2) / |y''|"}},
        "/* Radius of Curvature */\n// Cartesian: rho = [1 + (dy/dx)^2]^(3/2) / |d^2y/dx^2|\n// Curvature kappa = 1 / rho"
    },
    {
        58, "MATH 1107", 19, "[Pre-Mid] Lec 19: Curve Tracing", TOPIC_STATUS_COMPLETED, 1,
        {{"Curve tracing", TOPIC_STATUS_COMPLETED, "12/08/2026 (12/08/26) • Symmetry, origin, intercepts, asymptotes & regions"}},
        "/* Curve Tracing Guidelines */\n// 1. Symmetry (x-axis, y-axis, origin)\n// 2. Intercepts (x=0, y=0) & Tangents at origin\n// 3. Asymptotes & Regions of existence\n// Midterm PL begins 13/08/2026"
    },

    /* --- PART 2: POST-MID TERM (Lectures 20 to 39) --- Teacher: Mst. Munny Khatun --- */
    {
        59, "MATH 1107", 20, "[Post-Mid] Lec 20: Intro to Integral Calculus", TOPIC_STATUS_COMPLETED, 1,
        {{"Introduction of Integral Calculus", TOPIC_STATUS_COMPLETED, "10/09/2026 (10/09/26) • Fundamental theorem of calculus & anti-derivatives"}},
        "/* Fundamental Theorem of Calculus */\n// d/dx [ integral_a^x f(t) dt ] = f(x)\n// integral_a^b f(x) dx = F(b) - F(a), where F'(x) = f(x)"
    },
    {
        60, "MATH 1107", 21, "[Post-Mid] Lec 21: Integration by Substitution (I)", TOPIC_STATUS_COMPLETED, 1,
        {{"Integration by method of substitution", TOPIC_STATUS_COMPLETED, "13/09/2026 (13/09/26) • Algebraic substitutions, u = g(x)"}},
        "/* Integration by Substitution */\n// integral f(g(x)) * g'(x) dx = integral f(u) du\n// Example: integral [x / (x^2 + 1)] dx = (1/2)*ln(x^2 + 1) + C"
    },
    {
        61, "MATH 1107", 22, "[Post-Mid] Lec 22: Integration by Substitution (II)", TOPIC_STATUS_COMPLETED, 1,
        {{"Integration by method of substitution", TOPIC_STATUS_COMPLETED, "16/09/2026 (16/09/26) • Trigonometric substitutions (sin, tan, sec)"}},
        "/* Trigonometric Substitution */\n// sqrt(a^2 - x^2)  ==> x = a * sin(theta)\n// sqrt(a^2 + x^2)  ==> x = a * tan(theta)\n// sqrt(x^2 - a^2)  ==> x = a * sec(theta)"
    },
    {
        62, "MATH 1107", 23, "[Post-Mid] Lec 23: Integration by Parts (I)", TOPIC_STATUS_COMPLETED, 1,
        {{"Integration by parts", TOPIC_STATUS_COMPLETED, "17/09/2026 (17/09/26) • LIATE priority rule, int u*v dx"}},
        "/* Integration by Parts (LIATE Rule) */\n// integral u * v dx = u * integral v dx - integral [ (du/dx) * (integral v dx) ] dx\n// Priority: Log, Inverse trig, Algebraic, Trig, Exponential"
    },
    {
        63, "MATH 1107", 24, "[Post-Mid] Lec 24: Integration by Parts (II)", TOPIC_STATUS_COMPLETED, 1,
        {{"Integration by parts", TOPIC_STATUS_COMPLETED, "20/09/2026 (20/09/26) • Cyclic integrals: e^(ax)*sin(bx) & e^(ax)*cos(bx)"}},
        "/* Cyclic Integration by Parts */\n// integral e^(ax)*sin(bx) dx = [e^(ax)/(a^2 + b^2)] * [a*sin(bx) - b*cos(bx)] + C\n// integral e^(ax)*cos(bx) dx = [e^(ax)/(a^2 + b^2)] * [a*cos(bx) + b*sin(bx)] + C"
    },
    {
        64, "MATH 1107", 25, "[Post-Mid] Lec 25: Method of Successive Reduction (I)", TOPIC_STATUS_COMPLETED, 1,
        {{"The method of successive reduction", TOPIC_STATUS_COMPLETED, "23/09/2026 (23/09/26) • Reduction formulas for sin^n(x) & cos^n(x)"}},
        "/* Reduction Formula for sin^n(x) */\n// I_n = integral sin^n(x) dx\n// I_n = -[sin^{n-1}(x) * cos(x)] / n  +  [(n-1)/n] * I_{n-2}"
    },
    {
        65, "MATH 1107", 26, "[Post-Mid] Lec 26: Method of Successive Reduction (II)", TOPIC_STATUS_COMPLETED, 1,
        {{"The method of successive reduction", TOPIC_STATUS_COMPLETED, "24/09/2026 (24/09/26) • Reduction formulas for tan^n(x), sec^n(x), x^n*e^x"}},
        "/* Reduction Formulas */\n// integral tan^n(x) dx = tan^{n-1}(x)/(n-1) - integral tan^{n-2}(x) dx\n// integral sec^n(x) dx = sec^{n-2}(x)*tan(x)/(n-1) + [(n-2)/(n-1)]*integral sec^{n-2}(x) dx"
    },
    {
        66, "MATH 1107", 27, "[Post-Mid] Lec 27: Introduction of Definite Integrals", TOPIC_STATUS_COMPLETED, 1,
        {{"Introduction of definite integrals", TOPIC_STATUS_COMPLETED, "27/09/2026 (27/09/26) • Riemann sum definition, limits of partitions"}},
        "/* Definite Integral as Riemann Sum */\n// integral_a^b f(x) dx = lim_{n->inf} sum_{i=1}^{n} f(x_i*) * Delta x\n// where Delta x = (b - a) / n"
    },
    {
        67, "MATH 1107", 28, "[Post-Mid] Lec 28: Definite Integrals Properties", TOPIC_STATUS_COMPLETED, 1,
        {{"Definite integrals properties", TOPIC_STATUS_COMPLETED, "30/09/2026 (30/09/26) • King's rule, odd/even functions, periodic integrals"}},
        "/* Essential Properties of Definite Integrals */\n// 1. integral_0^a f(x) dx = integral_0^a f(a - x) dx  [King's Property]\n// 2. integral_{-a}^a f(x) dx = 2*integral_0^a f(x) dx  [even f]\n//                            = 0                       [odd f]"
    },
    {
        68, "MATH 1107", 29, "[Post-Mid] Lec 29: Summing Series via Definite Integrals", TOPIC_STATUS_COMPLETED, 1,
        {{"Definite integrals use in summing series", TOPIC_STATUS_COMPLETED, "01/10/2026 (01/10/26) • lim (1/n)*sum f(r/n) = int_0^1 f(x) dx"}},
        "/* Summation of Infinite Series */\n// lim_{n->inf} sum_{r=1}^{n} (1/n) * f(r/n) = integral_0^1 f(x) dx\n// Transform discrete sums into definite Riemann integrals."
    },
    {
        69, "MATH 1107", 30, "[Post-Mid] Lec 30: Wallis's Formula & Applications [CT-2]", TOPIC_STATUS_COMPLETED, 1,
        {{"Wallis's formula and applications", TOPIC_STATUS_COMPLETED, "04/10/2026 (04/10/26) ★ CLASS TEST 2 (CT-2) • Wallis's formula for sin^m(x)*cos^n(x)"}},
        "/* Wallis's Formula */\n// integral_0^{pi/2} sin^n(x) dx = [(n-1)/n]*[(n-3)/(n-2)]*...*(1/2)*(pi/2) [even n]\n//                               = [(n-1)/n]*[(n-3)/(n-2)]*...*(2/3)*1      [odd n]"
    },
    {
        70, "MATH 1107", 31, "[Post-Mid] Lec 31: Improper Integrals & Classifications", TOPIC_STATUS_COMPLETED, 1,
        {{"Improper integrals and its classifications", TOPIC_STATUS_COMPLETED, "07/10/2026 (07/10/26) • Type 1 (infinite limits) & Type 2 (unbounded integrands)"}},
        "/* Improper Integrals */\n// Type 1: integral_a^inf f(x) dx = lim_{t->inf} integral_a^t f(x) dx\n// Type 2: integral_a^b [1/(x-a)^p] dx converges if p < 1"
    },
    {
        71, "MATH 1107", 32, "[Post-Mid] Lec 32: Differentiation under Integral Sign", TOPIC_STATUS_COMPLETED, 1,
        {{"Differentiation under the sign of integration", TOPIC_STATUS_COMPLETED, "08/10/2026 (08/10/26) • Leibniz rule for parameter differentiation"}},
        "/* Leibniz Rule for Parameter Differentiation */\n// d/da [ integral_{u(a)}^{v(a)} f(x,a) dx ]\n//   = integral_u^v [partial f / partial a] dx  +  f(v,a)*(dv/da)  -  f(u,a)*(du/da)"
    },
    {
        72, "MATH 1107", 33, "[Post-Mid] Lec 33: Integration under Integral Sign", TOPIC_STATUS_COMPLETED, 1,
        {{"Integration under the sign of integration", TOPIC_STATUS_COMPLETED, "11/10/2026 (11/10/26) • Reversing integration order & Frullani integrals"}},
        "/* Frullani's Integral */\n// integral_0^inf [ [f(ax) - f(bx)] / x ] dx = [ f(0) - f(inf) ] * ln(b/a)"
    },
    {
        73, "MATH 1107", 34, "[Post-Mid] Lec 34: Beta & Gamma Functions Definitions", TOPIC_STATUS_COMPLETED, 1,
        {{"Beta and Gamma function: Definitions and properties", TOPIC_STATUS_COMPLETED, "14/10/2026 (14/10/26) • Gamma(n), Beta(m,n), Gamma(1/2)=sqrt(pi)"}},
        "/* Gamma and Beta Function Definitions */\n// Gamma(n) = integral_0^inf e^(-t) * t^{n-1} dt\n// Gamma(n+1) = n * Gamma(n);  Gamma(1/2) = sqrt(pi)\n// B(m,n) = [Gamma(m)*Gamma(n)] / Gamma(m+n)"
    },
    {
        74, "MATH 1107", 35, "[Post-Mid] Lec 35: Application of Beta & Gamma Functions", TOPIC_STATUS_IN_PROGRESS, 1,
        {{"Application of Beta and Gamma functions in evaluation", TOPIC_STATUS_IN_PROGRESS, "15/10/2026 (15/10/26) [TODAY] • Evaluation of sin^p(x)*cos^q(x) integrals via Beta"}},
        "/* Beta Function Evaluation */\n// integral_0^{pi/2} sin^{2m-1}(x) * cos^{2n-1}(x) dx = (1/2) * B(m, n)\n//                                              = [Gamma(m)*Gamma(n)] / [2*Gamma(m+n)]\n// Duplication Formula: Gamma(m)*Gamma(m + 1/2) = [sqrt(pi)/2^{2m-1}] * Gamma(2m)"
    },
    {
        75, "MATH 1107", 36, "[Post-Mid] Lec 36: Jacobian Determinants & Transformations", TOPIC_STATUS_PENDING, 1,
        {{"Jacobian", TOPIC_STATUS_PENDING, "25/10/2026 (25/10/26) • Coordinate transformations, polar / cylindrical Jacobian"}},
        "/* Jacobian Determinants */\n// J = d(x,y)/d(u,v) = | dx/du   dx/dv |\n//                     | dy/du   dy/dv |\n// dx dy = |J| du dv;  For polar (r, theta): J = r ==> dx dy = r dr dtheta"
    },
    {
        76, "MATH 1107", 37, "[Post-Mid] Lec 37: Multiple Integrals & Applications (I)", TOPIC_STATUS_PENDING, 1,
        {{"Multiple integrals and their applications-1", TOPIC_STATUS_PENDING, "28/10/2026 (28/10/26) • Double integrals, area & volume calculations"}},
        "/* Double Integrals */\n// Area:   A = double_integral_R dx dy = double_integral_R r dr dtheta\n// Volume: V = double_integral_R z(x, y) dx dy"
    },
    {
        77, "MATH 1107", 38, "[Post-Mid] Lec 38: Multiple Integrals & Applications (II)", TOPIC_STATUS_PENDING, 1,
        {{"Multiple integrals and their applications-2", TOPIC_STATUS_PENDING, "29/10/2026 (29/10/26) • Triple integrals, mass & center of gravity"}},
        "/* Triple Integrals */\n// Cartesian:   dV = dx dy dz\n// Cylindrical: dV = r dr dtheta dz\n// Spherical:   dV = rho^2 * sin(phi) d_rho d_phi d_theta"
    },
    {
        78, "MATH 1107", 39, "[Post-Mid] Lec 39: Final Comprehensive Review", TOPIC_STATUS_PENDING, 1,
        {{"Review", TOPIC_STATUS_PENDING, "01/11/2026 (01/11/26) • Comprehensive Review: Differential & Integral Calculus • PL begins 04/11"}},
        "/* Final Exam Review */\n// Pre-Mid: Lec 1-19 (Limits, Leibnitz, Taylor, Partial Diff, Asymptotes, Curvature)\n// Post-Mid: Lec 20-38 (Integration by Parts, Reduction, Wallis, Beta/Gamma, Multiple)\n// Term Final Exam: 12/11/2026 (10:00 AM, CSE 101)"
    }
};

void storage_reset_kuet_defaults(void) {
    memset(&g_state, 0, sizeof(AppState));

    /* 1. Default Settings */
    g_state.settings.is_24_hour = false;
    g_state.settings.first_day_of_week = 0; /* Sunday */
    g_state.settings.countdown_style = 0;   /* Detailed ("05 Days 12 Hours 21 Minutes") */
    g_state.settings.reminder_minutes = 10;
    g_state.settings.color_theme = 0;       /* Cyan Tech */
    g_state.settings.enable_colors = true;
    g_state.settings.show_seconds = true;

    /* 2. Authentic KUET CSE Routine Courses (Section B / B1) */
    Course sample_courses[] = {
        {1, "CSE 1101", "Structured Programming", "Md Tajmilur Rahman / Ehsanul Karim", 3.00f, "Green", "Pre-Mid: Md Tajmilur Rahman | Post-Mid: Ehsanul Karim (3.0 Credits)"},
        {2, "CSE 1102", "Structured Programming Lab", "", 1.50f, "Blue", "Hands-on C laboratory & problem solving"},
        {3, "CSE 1107", "Discrete Mathematics", "", 3.00f, "Cyan", "Logic, Sets, Relations, Graph Theory"},
        {4, "MATH 1107", "Differential and Integral Calculus", "Md. Dulal Hossain / Mst. Munny Khatun", 3.00f, "Yellow", "Pre-Mid: Md. Dulal Hossain | Post-Mid: Mst. Munny Khatun (3.0 Credits)"},
        {5, "PHY 1107", "Physics", "", 3.00f, "Magenta", "Optics, waves, electromagnetism"},
        {6, "PHY 1108", "Physics Laboratory", "", 0.75f, "Magenta", "Experimental physics and optics lab"},
        {7, "HUM 1107", "Humanities (English & Economics)", "", 3.00f, "Yellow", "English communication, technical writing, economics"},
        {8, "HUM 1108", "English Skills Laboratory", "", 0.75f, "Yellow", "Language lab, listening, speaking & presentations"}
    };
    g_state.course_count = sizeof(sample_courses) / sizeof(sample_courses[0]);
    memcpy(g_state.courses, sample_courses, sizeof(sample_courses));

    /* 3. Authentic KUET CSE Routine Classes (SEC - B, Group B1) */
    RoutineClass sample_classes[] = {
        /* Sunday (Room: CSE 101) */
        {1,  0, "MATH 1107", "Differential and Integral Calculus", 10, 40, 11, 30, "CSE 101", "Md. Dulal Hossain / Mst. Munny Khatun", CLASS_TYPE_THEORY, true},
        {2,  0, "CSE 1101",  "Structured Programming",              11, 30, 12, 20, "CSE 101", "Md Tajmilur Rahman / Ehsanul Karim", CLASS_TYPE_THEORY, true},
        {3,  0, "CSE 1107",  "Discrete Mathematics",                15, 20, 17,  0, "CSE 101", "", CLASS_TYPE_THEORY, true},

        /* Monday (Room: CSE 101, Lab: Physics Lab) */
        {4,  1, "PHY 1107",  "Physics",                             8,  50, 9,  40, "CSE 101", "", CLASS_TYPE_THEORY, true},
        {5,  1, "PHY 1108",  "Physics Laboratory (B1)",             11, 30, 13, 10, "Physics Lab", "", CLASS_TYPE_LAB, true},

        /* Tuesday (Room: CSE 101) */
        {6,  2, "CSE 1101",  "Structured Programming",              8,  50, 9,  40, "CSE 101", "Md Tajmilur Rahman / Ehsanul Karim", CLASS_TYPE_THEORY, true},
        {7,  2, "MATH 1107", "Differential and Integral Calculus",  9,  40, 10, 30, "CSE 101", "Md. Dulal Hossain / Mst. Munny Khatun", CLASS_TYPE_THEORY, true},
        {8,  2, "HUM 1107",  "Humanities",                          11, 30, 12, 20, "CSE 101", "", CLASS_TYPE_THEORY, true},
        {9,  2, "CSE 1107",  "Discrete Mathematics",                12, 20, 13, 10, "CSE 101", "", CLASS_TYPE_THEORY, true},

        /* Wednesday (Room: CSE 101, Lab: Language Lab) */
        {10, 3, "CSE 1101",  "Structured Programming",              10, 40, 11, 30, "CSE 101", "Md Tajmilur Rahman / Ehsanul Karim", CLASS_TYPE_THEORY, true},
        {11, 3, "PHY 1107",  "Physics",                             11, 30, 12, 20, "CSE 101", "", CLASS_TYPE_THEORY, true},
        {12, 3, "HUM 1107",  "Humanities",                          12, 20, 13, 10, "CSE 101", "", CLASS_TYPE_THEORY, true},
        {13, 3, "HUM 1108",  "English Skills Laboratory (B1)",      14, 30, 17,  0, "Language Lab", "", CLASS_TYPE_LAB, true},

        /* Thursday (Theory in CSE 102, Lab in CSE 104) */
        {14, 4, "HUM 1107",  "Humanities",                          10, 40, 11, 30, "CSE 102", "", CLASS_TYPE_THEORY, true},
        {15, 4, "PHY 1107",  "Physics",                             11, 30, 12, 20, "CSE 102", "", CLASS_TYPE_THEORY, true},
        {16, 4, "MATH 1107", "Differential and Integral Calculus",  12, 20, 13, 10, "CSE 102", "Md. Dulal Hossain / Mst. Munny Khatun", CLASS_TYPE_THEORY, true},
        {17, 4, "CSE 1101",  "Structured Programming",              14, 20, 15,  0, "CSE 102", "Md Tajmilur Rahman / Ehsanul Karim", CLASS_TYPE_THEORY, true},
        {18, 4, "CSE 1102",  "Structured Programming Lab (B1)",     14, 30, 17, 30, "CSE 104", "", CLASS_TYPE_LAB, true}
    };
    g_state.class_count = sizeof(sample_classes) / sizeof(sample_classes[0]);
    memcpy(g_state.classes, sample_classes, sizeof(sample_classes));

    /* 4. Preloaded Exams (From Official Academic Calendar & Lecture Plan) */
    Exam sample_exams[] = {
        /* Midterm Examinations (18/08/2026 - 02/09/2026) */
        {
            1, "CSE 1101", "Structured Programming", "Midterm: Structured Programming", EXAM_TYPE_MIDTERM,
            2026, 8, 18, 10, 0, "CSE 101", 30.0f, 30.0f, true, true, "Midterm Exam Date 1 (Tuesday). Pre-Mid: Lec 1-18"
        },
        {
            2, "MATH 1107", "Differential and Integral Calculus", "Midterm: Differential Calculus", EXAM_TYPE_MIDTERM,
            2026, 8, 22, 10, 0, "CSE 101", 30.0f, 30.0f, true, true, "Midterm Exam Date 2 (Saturday). Pre-Mid: Lec 1-19 (PL: 13/08 - 17/08/26)"
        },
        {
            3, "PHY 1107", "Physics", "Midterm: Physics", EXAM_TYPE_MIDTERM,
            2026, 8, 25, 10, 0, "CSE 101", 30.0f, 30.0f, true, true, "Midterm Exam Date 3 (Tuesday). Optics, Waves, Electromagnetism"
        },
        {
            4, "HUM 1107", "Humanities", "Midterm: Humanities", EXAM_TYPE_MIDTERM,
            2026, 8, 29, 10, 0, "CSE 101", 30.0f, 30.0f, true, true, "Midterm Exam Date 4 (Saturday). English & Economics"
        },
        {
            5, "CSE 1107", "Discrete Mathematics", "Midterm: Discrete Mathematics", EXAM_TYPE_MIDTERM,
            2026, 9, 2, 10, 0, "CSE 101", 30.0f, 30.0f, true, true, "Midterm Exam Date 5 (Wednesday). Logic, Sets, Relations"
        },
        /* Class Test & Lab Milestone */
        {
            6, "CSE 1101", "Structured Programming", "CT-2: User Defined Data Types & Structs", EXAM_TYPE_CT,
            2026, 10, 4, 10, 40, "CSE 101", 20.0f, 15.0f, false, true, "Lec 19-29 milestone (Teacher: Ehsanul Karim)"
        },
        {
            7, "CSE 1102", "Structured Programming Lab", "Practical Lab Assessment", EXAM_TYPE_LAB_EXAM,
            2026, 10, 22, 15, 0, "CSE 104", 50.0f, 25.0f, false, true, "Live code evaluation on structs, recursion, and file I/O"
        },
        /* Term Final Examinations (09/11/2026 - 23/11/2026) */
        {
            8, "CSE 1101", "Structured Programming", "Term Final: Structured Programming", EXAM_TYPE_FINAL,
            2026, 11, 9, 10, 0, "CSE 101", 60.0f, 40.0f, false, true, "Term Final Date 1 (Monday). Comprehensive: All 39 Lectures"
        },
        {
            9, "MATH 1107", "Differential and Integral Calculus", "Term Final: Differential and Integral Calculus", EXAM_TYPE_FINAL,
            2026, 11, 12, 10, 0, "CSE 101", 60.0f, 40.0f, false, true, "Term Final Date 2 (Thursday). Comprehensive: All 39 Lectures (PL: 04/11 - 08/11/26)"
        },
        {
            10, "PHY 1107", "Physics", "Term Final: Physics", EXAM_TYPE_FINAL,
            2026, 11, 15, 10, 0, "CSE 101", 60.0f, 40.0f, false, true, "Term Final Date 3 (Sunday). Comprehensive Physics"
        },
        {
            11, "HUM 1107", "Humanities", "Term Final: Humanities", EXAM_TYPE_FINAL,
            2026, 11, 19, 10, 0, "CSE 101", 60.0f, 40.0f, false, true, "Term Final Date 4 (Thursday). English & Economics"
        },
        {
            12, "CSE 1107", "Discrete Mathematics", "Term Final: Discrete Mathematics", EXAM_TYPE_FINAL,
            2026, 11, 23, 10, 0, "CSE 101", 60.0f, 40.0f, false, true, "Term Final Date 5 (Monday). Comprehensive Discrete Mathematics"
        },
        /* MATH 1107 Class Tests from Lecture Plan */
        {
            13, "MATH 1107", "Differential and Integral Calculus", "CT-1: Partial Differentiation & Total Derivatives", EXAM_TYPE_CT,
            2026, 7, 22, 9, 40, "CSE 101", 20.0f, 18.0f, true, true, "Lecture 11 Milestone (Teacher: Md. Dulal Hossain)"
        },
        {
            14, "MATH 1107", "Differential and Integral Calculus", "CT-2: Wallis's Formula & Applications", EXAM_TYPE_CT,
            2026, 10, 4, 10, 40, "CSE 101", 20.0f, 16.5f, true, true, "Lecture 30 Milestone (Teacher: Mst. Munny Khatun)"
        }
    };
    g_state.exam_count = sizeof(sample_exams) / sizeof(sample_exams[0]);
    memcpy(g_state.exams, sample_exams, sizeof(sample_exams));

    /* 5. Syllabus with Tree Hierarchy (Official CSE 1101 Lecture Plan) */
    SyllabusTopic sample_topics[] = {
        /* --- CSE 1101 PART 1: PRE-MID TERM (Lectures 01 to 18) --- Teacher: Md Tajmilur Rahman */
        {
            1, "CSE 1101", 1, "[Pre-Mid] Lec 01: Programming & Paradigms", TOPIC_STATUS_COMPLETED, 1,
            {{"Intro to programming, level of languages, paradigms", TOPIC_STATUS_COMPLETED, "28/06/2026 (B) | 29/06/2026 (A)"}},
            ""
        },
        {
            2, "CSE 1101", 2, "[Pre-Mid] Lec 02: Structured Lang & Compilation", TOPIC_STATUS_COMPLETED, 1,
            {{"Structured programming, control structures, compilation", TOPIC_STATUS_COMPLETED, "30/06/2026 (A & B) • Steps of compilation"}},
            ""
        },
        {
            3, "CSE 1101", 3, "[Pre-Mid] Lec 03: Tokens, Types & Variables", TOPIC_STATUS_COMPLETED, 1,
            {{"Primary types (int, float, char, double), memory sizes", TOPIC_STATUS_COMPLETED, "01/07/2026 (A) | 02/07/2026 (B) • Format specifiers"}},
            "/* Format Specifiers Example */\n#include <stdio.h>\nint main() {\n    int a = 10;\n    float b = 3.14f;\n    char c = 'K';\n    printf(\"%d, %.2f, %c\\n\", a, b, c);\n    return 0;\n}"
        },
        {
            4, "CSE 1101", 4, "[Pre-Mid] Lec 04: Overflow, Constants & Memory", TOPIC_STATUS_COMPLETED, 1,
            {{"Data type overflow, variables, constants, memory concept", TOPIC_STATUS_COMPLETED, "05/07/2026 (B) | 06/07/2026 (A)"}},
            ""
        },
        {
            5, "CSE 1101", 5, "[Pre-Mid] Lec 05: Operators: Arithmetic to Ternary", TOPIC_STATUS_COMPLETED, 1,
            {{"Arithmetic, relational, logical, bitwise, ternary ops", TOPIC_STATUS_COMPLETED, "07/07/2026 (A & B) • Assignment & increment/decrement"}},
            "/* Bitwise Operations */\nint a = 5, b = 9;\nint bit_and = a & b;\nint bit_xor = a ^ b;\nint shifted = a << 1;"
        },
        {
            6, "CSE 1101", 6, "[Pre-Mid] Lec 06: Precedence & Type Conversion", TOPIC_STATUS_COMPLETED, 1,
            {{"Operator precedence & associativity, type conversion", TOPIC_STATUS_COMPLETED, "08/07/2026 (A) | 09/07/2026 (B) • Explicit & implicit"}},
            ""
        },
        {
            7, "CSE 1101", 7, "[Pre-Mid] Lec 07: Conditional Branch Pipelines", TOPIC_STATUS_COMPLETED, 1,
            {{"Branch pipelines using if, if-else, nested conditions", TOPIC_STATUS_COMPLETED, "12/07/2026 (B) | 13/07/2026 (A)"}},
            ""
        },
        {
            8, "CSE 1101", 8, "[Pre-Mid] Lec 08: Multi-way Branching (Switch-case)", TOPIC_STATUS_COMPLETED, 1,
            {{"Multi-way branching using switch-case, goto statements", TOPIC_STATUS_COMPLETED, "14/07/2026 (A & B)"}},
            ""
        },
        {
            9, "CSE 1101", 9, "[Pre-Mid] Lec 09: Iteration: for, while, do-while", TOPIC_STATUS_COMPLETED, 1,
            {{"Repetitive blocks & conditional evaluations using loops", TOPIC_STATUS_COMPLETED, "15/07/2026 (A) | 16/07/2026 (B)"}},
            "/* Standard for-loop */\nfor (int i = 0; i < n; i++) {\n    printf(\"%d \", i);\n}"
        },
        {
            10, "CSE 1101", 10, "[Pre-Mid] Lec 10: Loop Optimization & Practice", TOPIC_STATUS_COMPLETED, 1,
            {{"Break and continue, nested loops, practice problems", TOPIC_STATUS_COMPLETED, "19/07/2026 (B) | 20/07/2026 (A)"}},
            ""
        },
        {
            11, "CSE 1101", 11, "[Pre-Mid] Lec 11: Modular Functions & Prototypes", TOPIC_STATUS_COMPLETED, 1,
            {{"Modular design, function declarations, definitions, calls", TOPIC_STATUS_COMPLETED, "21/07/2026 (A & B)"}},
            ""
        },
        {
            12, "CSE 1101", 12, "[Pre-Mid] Lec 12: Parameter Passing Conventions", TOPIC_STATUS_COMPLETED, 1,
            {{"Call-by-Value mechanics vs reference-passing layouts", TOPIC_STATUS_COMPLETED, "22/07/2026 (A) | 23/07/2026 (B)"}},
            ""
        },
        {
            13, "CSE 1101", 13, "[Pre-Mid] Lec 13: Recursion & Stack Frames [CT-1]", TOPIC_STATUS_COMPLETED, 1,
            {{"Self-invocation, base conditions, execution frames", TOPIC_STATUS_COMPLETED, "26/07/2026 (B) | 27/07/2026 (A) ★ Class Test 1"}},
            "/* Recursive Factorial with Execution Frame */\nlong long fact(int n) {\n    if (n <= 1) return 1;\n    return n * fact(n - 1);\n}"
        },
        {
            14, "CSE 1101", 14, "[Pre-Mid] Lec 14: Scope Rules & Storage Classes", TOPIC_STATUS_COMPLETED, 1,
            {{"Local vs global scopes; auto, extern, register, static", TOPIC_STATUS_COMPLETED, "28/07/2026 (A & B)"}},
            ""
        },
        {
            15, "CSE 1101", 15, "[Pre-Mid] Lec 15: File Access: Streams & Modes", TOPIC_STATUS_COMPLETED, 1,
            {{"File control blocks, stream pointers, modes (r,w,a,rb,wb)", TOPIC_STATUS_COMPLETED, "29/07/2026 (A) | 30/07/2026 (B)"}},
            ""
        },
        {
            16, "CSE 1101", 16, "[Pre-Mid] Lec 16: File Processing: Text & Binary", TOPIC_STATUS_COMPLETED, 1,
            {{"Reading/writing pipelines: fread, fwrite, fprintf, fscanf", TOPIC_STATUS_COMPLETED, "02/08/2026 (B) | 03/08/2026 (A)"}},
            "/* Binary File Block Write */\nFILE *fp = fopen(\"data.bin\", \"wb\");\nif (fp) {\n    fwrite(buffer, sizeof(Item), count, fp);\n    fclose(fp);\n}"
        },
        {
            17, "CSE 1101", 17, "[Pre-Mid] Lec 17: Command Line Parameters", TOPIC_STATUS_COMPLETED, 1,
            {{"Parsing program launch variables via argc and argv", TOPIC_STATUS_COMPLETED, "04/08/2026 (A & B)"}},
            "int main(int argc, char* argv[]) {\n    for (int i = 0; i < argc; i++) {\n        printf(\"arg[%d] = %s\\n\", i, argv[i]);\n    }\n    return 0;\n}"
        },
        {
            18, "CSE 1101", 18, "[Pre-Mid] Lec 18: Preprocessor & Header Files", TOPIC_STATUS_COMPLETED, 1,
            {{"Macros (#define), inclusions (#include), custom headers", TOPIC_STATUS_COMPLETED, "05/08/2026 (A) | 06/08/2026 (B) • Midterm PL"}},
            ""
        },

        /* --- CSE 1101 PART 2: POST-MID TERM (Lectures 19 to 39) --- Teacher: Ehsanul Karim */
        {
            19, "CSE 1101", 19, "[Post-Mid] Lec 19: Arrays & Contiguous Memory", TOPIC_STATUS_COMPLETED, 1,
            {{"Contiguous memory blocks, index access, bounds checking", TOPIC_STATUS_COMPLETED, "10/09/2026 (A & B) • Post-Midterm Start"}},
            ""
        },
        {
            20, "CSE 1101", 20, "[Post-Mid] Lec 20: One-Dimensional Arrays", TOPIC_STATUS_COMPLETED, 1,
            {{"Declaration, initialization, aggregate computations", TOPIC_STATUS_COMPLETED, "13/09/2026 (B) | 14/09/2026 (A)"}},
            ""
        },
        {
            21, "CSE 1101", 21, "[Post-Mid] Lec 21: Multidimensional Arrays", TOPIC_STATUS_COMPLETED, 1,
            {{"Row-major memory organization, matrix computation", TOPIC_STATUS_COMPLETED, "15/09/2026 (A & B)"}},
            "/* 2D Matrix Row-Major Access */\nint mat[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};\nfor (int r = 0; r < 3; r++) {\n    for (int c = 0; c < 3; c++) {\n        printf(\"%d \", mat[r][c]);\n    }\n    printf(\"\\n\");\n}"
        },
        {
            22, "CSE 1101", 22, "[Post-Mid] Lec 22: Arrays in Functions", TOPIC_STATUS_COMPLETED, 1,
            {{"Passing arrays to functions, size management, modularity", TOPIC_STATUS_COMPLETED, "16/09/2026 (A) | 17/09/2026 (B)"}},
            ""
        },
        {
            23, "CSE 1101", 23, "[Post-Mid] Lec 23: Array Search & Sort Algorithms", TOPIC_STATUS_COMPLETED, 1,
            {{"Linear search, binary search, sorting, frequency counting", TOPIC_STATUS_COMPLETED, "20/09/2026 (B) | 21/09/2026 (A)"}},
            ""
        },
        {
            24, "CSE 1101", 24, "[Post-Mid] Lec 24: Pointers & Memory Addressing", TOPIC_STATUS_COMPLETED, 1,
            {{"Pointers declaration, address-of (&), dereference (*)", TOPIC_STATUS_COMPLETED, "22/09/2026 (A & B) • NULL pointer safety"}},
            "/* Pointers & Dereferencing */\nint x = 42;\nint *ptr = &x;\nprintf(\"Address: %p, Value: %d\\n\", (void*)ptr, *ptr);"
        },
        {
            25, "CSE 1101", 25, "[Post-Mid] Lec 25: Pointer Arithmetic & Offsets", TOPIC_STATUS_COMPLETED, 1,
            {{"Pointer increments, address offsets, pointer comparison", TOPIC_STATUS_COMPLETED, "23/09/2026 (A) | 24/09/2026 (B)"}},
            ""
        },
        {
            26, "CSE 1101", 26, "[Post-Mid] Lec 26: Arrays and Pointer Relationships", TOPIC_STATUS_COMPLETED, 1,
            {{"Array names as base addresses, pointers to arrays", TOPIC_STATUS_COMPLETED, "27/09/2026 (B) | 28/09/2026 (A)"}},
            ""
        },
        {
            27, "CSE 1101", 27, "[Post-Mid] Lec 27: Strings Fundamentals", TOPIC_STATUS_IN_PROGRESS, 1,
            {{"Character arrays, null-terminated strings (\\0), string I/O", TOPIC_STATUS_IN_PROGRESS, "29/09/2026 (A & B)"}},
            ""
        },
        {
            28, "CSE 1101", 28, "[Post-Mid] Lec 28: String Processing Algorithms", TOPIC_STATUS_IN_PROGRESS, 1,
            {{"Manual strlen, strcpy, strcmp, palindrome, substring", TOPIC_STATUS_IN_PROGRESS, "30/09/2026 (A) | 01/10/2026 (B)"}},
            "/* Manual String Length */\nsize_t my_strlen(const char* s) {\n    const char* p = s;\n    while (*p) p++;\n    return p - s;\n}"
        },
        {
            29, "CSE 1101", 29, "[Post-Mid] Lec 29: User Defined Types [CT-2]", TOPIC_STATUS_PENDING, 1,
            {{"Custom data representation, struct declaration, members", TOPIC_STATUS_PENDING, "04/10/2026 (B) | 05/10/2026 (A) ★ Class Test 2"}},
            "/* Struct Declaration in C */\ntypedef struct {\n    int id;\n    char name[64];\n    float cgpa;\n} Student;"
        },
        {
            30, "CSE 1101", 30, "[Post-Mid] Lec 30: Structures in Program Design", TOPIC_STATUS_PENDING, 1,
            {{"Arrays of structures, structure pointers, record mgmt", TOPIC_STATUS_PENDING, "06/10/2026 (A & B)"}},
            ""
        },
        {
            31, "CSE 1101", 31, "[Post-Mid] Lec 31: Structure Bit Fields", TOPIC_STATUS_PENDING, 1,
            {{"Bit-level data storage, compact flags, memory efficiency", TOPIC_STATUS_PENDING, "07/10/2026 (A) | 08/10/2026 (B)"}},
            ""
        },
        {
            32, "CSE 1101", 32, "[Post-Mid] Lec 32: Structure Padding & Alignment", TOPIC_STATUS_PENDING, 1,
            {{"Memory alignment rules, padding bytes, packing issues", TOPIC_STATUS_PENDING, "11/10/2026 (B) | 12/10/2026 (A)"}},
            ""
        },
        {
            33, "CSE 1101", 33, "[Post-Mid] Lec 33: Unions and Enumerations", TOPIC_STATUS_PENDING, 1,
            {{"Shared memory representation, tagged unions, enum constants", TOPIC_STATUS_PENDING, "13/10/2026 (A & B)"}},
            ""
        },
        {
            34, "CSE 1101", 34, "[Post-Mid] Lec 34: Dynamic Memory Allocation", TOPIC_STATUS_PENDING, 1,
            {{"malloc, calloc, realloc, free, heap usage, memory leaks", TOPIC_STATUS_PENDING, "14/10/2026 (A) | 15/10/2026 (B)"}},
            "/* Dynamic Allocation in C */\nint *arr = (int*)malloc(n * sizeof(int));\nif (arr != NULL) {\n    arr[0] = 42;\n    free(arr);\n    arr = NULL;\n}"
        },
        {
            35, "CSE 1101", 35, "[Post-Mid] Lec 35: Error Handling and Debugging", TOPIC_STATUS_PENDING, 1,
            {{"Runtime error detection, assertions, debugger tracing", TOPIC_STATUS_PENDING, "25/10/2026 (B) | 26/10/2026 (A)"}},
            ""
        },
        {
            36, "CSE 1101", 36, "[Post-Mid] Lec 36: Build Tools: Make & Valgrind", TOPIC_STATUS_PENDING, 1,
            {{"Compilation workflow, Makefiles, Valgrind checking", TOPIC_STATUS_PENDING, "27/10/2026 (A & B)"}},
            ""
        },
        {
            37, "CSE 1101", 37, "[Post-Mid] Lec 37: Graphics Routines Setup", TOPIC_STATUS_PENDING, 1,
            {{"Basic graphics library setup, coordinates, visualization", TOPIC_STATUS_PENDING, "28/10/2026 (A) | 29/10/2026 (B)"}},
            ""
        },
        {
            38, "CSE 1101", 38, "[Post-Mid] Lec 38: Final Exam Revision (Pre-Mid)", TOPIC_STATUS_PENDING, 1,
            {{"Review of pre-midterm fundamentals (Lectures 1-18)", TOPIC_STATUS_PENDING, "01/11/2026 (B) | 02/11/2026 (A)"}},
            ""
        },
        {
            39, "CSE 1101", 39, "[Post-Mid] Lec 39: Final Exam Revision (Post-Mid)", TOPIC_STATUS_PENDING, 1,
            {{"Review of post-midterm advanced C (Lectures 19-37)", TOPIC_STATUS_PENDING, "03/11/2026 (A & B) • PL starts 04/11"}},
            ""
        }
    };
    int cse_count = sizeof(sample_topics) / sizeof(sample_topics[0]);
    int math_count = sizeof(g_math_topics) / sizeof(g_math_topics[0]);
    memcpy(g_state.syllabus, sample_topics, sizeof(sample_topics));
    memcpy(g_state.syllabus + cse_count, g_math_topics, sizeof(g_math_topics));
    g_state.syllabus_count = cse_count + math_count;

    /* 6. Academic Grades for GPA Calculation (Empty by default) */
    g_state.grade_count = 0;

    storage_save_all();
}

bool storage_init_all(void) {
    ensure_data_directory();

    /* Try loading courses; if not found, initialize authentic defaults */
    if (!storage_load_courses() || g_state.course_count == 0) {
        storage_reset_kuet_defaults();
        return true;
    }

    if (!storage_load_routine() || g_state.class_count == 0) {
        storage_reset_kuet_defaults();
        return true;
    }
    if (!storage_load_exams() || g_state.exam_count == 0) {
        storage_reset_kuet_defaults();
        return true;
    }
    storage_load_syllabus();
    storage_load_grades();

    /* Strictly keep ONLY the courses present in the routine, removing everything else */
    if (g_state.class_count > 0) {
        Course filtered_courses[MAX_COURSES];
        int new_course_count = 0;
        for (int i = 0; i < g_state.course_count; i++) {
            bool in_routine = false;
            for (int c = 0; c < g_state.class_count; c++) {
                if (_stricmp(g_state.courses[i].code, g_state.classes[c].course_code) == 0) {
                    in_routine = true;
                    break;
                }
            }
            if (in_routine) {
                filtered_courses[new_course_count] = g_state.courses[i];
                filtered_courses[new_course_count].id = new_course_count + 1;
                new_course_count++;
            }
        }
        if (new_course_count != g_state.course_count) {
            g_state.course_count = new_course_count;
            memcpy(g_state.courses, filtered_courses, sizeof(Course) * new_course_count);
            storage_save_courses();
        }

        /* Filter exams to match routine courses only */
        Exam filtered_exams[MAX_EXAMS];
        int new_exam_count = 0;
        for (int i = 0; i < g_state.exam_count; i++) {
            bool in_routine = false;
            for (int c = 0; c < g_state.class_count; c++) {
                if (_stricmp(g_state.exams[i].course_code, g_state.classes[c].course_code) == 0) {
                    in_routine = true;
                    break;
                }
            }
            if (in_routine) {
                filtered_exams[new_exam_count] = g_state.exams[i];
                filtered_exams[new_exam_count].id = new_exam_count + 1;
                new_exam_count++;
            }
        }
        if (new_exam_count != g_state.exam_count) {
            g_state.exam_count = new_exam_count;
            memcpy(g_state.exams, filtered_exams, sizeof(Exam) * new_exam_count);
            storage_save_exams();
        }
    }

    /* Auto-sync MATH 1107: Differential and Integral Calculus from official Lecture Plan */
    bool has_math_syllabus = false;
    for (int i = 0; i < g_state.syllabus_count; i++) {
        if (_stricmp(g_state.syllabus[i].course_code, "MATH 1107") == 0) {
            has_math_syllabus = true;
            break;
        }
    }
    if (!has_math_syllabus) {
        int math_count = sizeof(g_math_topics) / sizeof(g_math_topics[0]);
        for (int m = 0; m < math_count && g_state.syllabus_count < MAX_TOPICS; m++) {
            g_state.syllabus[g_state.syllabus_count++] = g_math_topics[m];
        }
        storage_save_syllabus();
    }

    /* Sync MATH 1107 Course info & teachers */
    for (int i = 0; i < g_state.course_count; i++) {
        if (_stricmp(g_state.courses[i].code, "MATH 1107") == 0) {
            strncpy(g_state.courses[i].teacher, "Md. Dulal Hossain / Mst. Munny Khatun", STR_NAME_LEN - 1);
            strncpy(g_state.courses[i].notes, "Pre-Mid: Md. Dulal Hossain | Post-Mid: Mst. Munny Khatun (3.0 Credits)", STR_MED_LEN - 1);
            storage_save_courses();
            break;
        }
    }

    /* Sync MATH 1107 routine classes */
    bool routine_sync = false;
    for (int i = 0; i < g_state.class_count; i++) {
        if (_stricmp(g_state.classes[i].course_code, "MATH 1107") == 0) {
            strncpy(g_state.classes[i].teacher, "Md. Dulal Hossain / Mst. Munny Khatun", STR_NAME_LEN - 1);
            routine_sync = true;
        }
    }
    if (routine_sync) {
        storage_save_routine();
    }

    /* Sync MATH 1107 exams & milestones */
    bool has_math_ct1 = false, has_math_ct2 = false, has_math_midterm = false;
    for (int i = 0; i < g_state.exam_count; i++) {
        if (_stricmp(g_state.exams[i].course_code, "MATH 1107") == 0) {
            if (strstr(g_state.exams[i].title, "CT-1")) has_math_ct1 = true;
            if (strstr(g_state.exams[i].title, "CT-2")) has_math_ct2 = true;
            if (g_state.exams[i].type == EXAM_TYPE_MIDTERM) {
                has_math_midterm = true;
                strncpy(g_state.exams[i].title, "Midterm: Differential Calculus", STR_TITLE_LEN - 1);
                strncpy(g_state.exams[i].notes, "Midterm Exam Date 2 (Saturday). Pre-Mid: Lec 1-19 (PL: 13/08 - 17/08/26)", STR_MED_LEN - 1);
            }
            if (g_state.exams[i].type == EXAM_TYPE_FINAL) {
                strncpy(g_state.exams[i].title, "Term Final: Differential and Integral Calculus", STR_TITLE_LEN - 1);
                strncpy(g_state.exams[i].notes, "Term Final Date 2 (Thursday). Comprehensive: All 39 Lectures (PL: 04/11 - 08/11/26)", STR_MED_LEN - 1);
            }
        }
    }
    bool exams_added = false;
    if (!has_math_midterm && g_state.exam_count < MAX_EXAMS) {
        Exam mid = {
            g_state.exam_count + 1, "MATH 1107", "Differential and Integral Calculus",
            "Midterm: Differential Calculus", EXAM_TYPE_MIDTERM,
            2026, 8, 22, 10, 0, "CSE 101", 30.0f, 30.0f, true, true,
            "Midterm Exam Date 2 (Saturday). Pre-Mid: Lec 1-19 (PL: 13/08 - 17/08/26)"
        };
        g_state.exams[g_state.exam_count++] = mid;
        exams_added = true;
    }
    if (!has_math_ct1 && g_state.exam_count < MAX_EXAMS) {
        Exam ct1 = {
            g_state.exam_count + 1, "MATH 1107", "Differential and Integral Calculus",
            "CT-1: Partial Differentiation & Total Derivatives", EXAM_TYPE_CT,
            2026, 7, 22, 9, 40, "CSE 101", 20.0f, 18.0f, true, true,
            "Lecture 11 Milestone (Teacher: Md. Dulal Hossain)"
        };
        g_state.exams[g_state.exam_count++] = ct1;
        exams_added = true;
    }
    if (!has_math_ct2 && g_state.exam_count < MAX_EXAMS) {
        Exam ct2 = {
            g_state.exam_count + 1, "MATH 1107", "Differential and Integral Calculus",
            "CT-2: Wallis's Formula & Applications", EXAM_TYPE_CT,
            2026, 10, 4, 10, 40, "CSE 101", 20.0f, 16.5f, true, true,
            "Lecture 30 Milestone (Teacher: Mst. Munny Khatun)"
        };
        g_state.exams[g_state.exam_count++] = ct2;
        exams_added = true;
    }
    if (exams_added) {
        storage_save_exams();
    }

    if (!storage_load_settings()) {
        g_state.settings.is_24_hour = false;
        g_state.settings.first_day_of_week = 0;
        g_state.settings.countdown_style = 0;
        g_state.settings.reminder_minutes = 10;
        g_state.settings.color_theme = 0;
        g_state.settings.enable_colors = true;
        g_state.settings.show_seconds = true;
        storage_save_settings();
    }
    return true;
}

import math;

def log10(x) :
    return math.log(x) / math.log(10);

def str_x_or_lg(nm, x) :   # def. lg(x) := log10(x)
    if x < 1e200 :
        return nm + " " + str(x);
    return "lg " + nm + " " + str(log10(x));

part = int(input("Partial ? (1=yes, 0=no) > "));
Q = int(input("Start Q > "));

A = 1;
B = 1;
QS = 1;
M = 0;
Q0 = Q;
QQ = [];
br = 0;

# first term
while A*Q >= B :
    print(str_x_or_lg("A", A), str_x_or_lg("B", B));

    M = M + 1;
    tmpA = A * Q - B;
    B = B * Q + A;
    A = tmpA;

# check other approximation for the first term
tmpA = A * Q - B;
tmpB = B * Q + A;
if A * tmpB + B * tmpA > 0 :
    QS = -1;
    M = M + 1;
    A = -tmpA;
    B = tmpB;
    print(str_x_or_lg("A", A), str_x_or_lg("B", B));

print("M", M, "\n---");

# higher terms
while A > 0 :
    print(str_x_or_lg("A", A), str_x_or_lg("B", B));

    Q = (B + A - 1) // A;

    tmpA = A * Q - B;
    tmpA1 = tmpA - A;

    if abs(tmpA) > abs(tmpA1) :
        Q = Q - 1;
        tmpA = tmpA1;

    B = B * Q + A;
    A = tmpA;

    QQ.append(QS * Q);  # trace the sign in q_k

    if A < 0 :
        A = -A;
        QS = -QS;

    if part > 0 and log10(Q) > 1000000 :
        print("break");
        br = 1;
        break;  # for partial series

# Lehmer measure
L = 1 / log10(Q0);
print("---\nM", M, str_x_or_lg("Q", Q0));
for q1 in QQ :
    L = L + 1 / log10(abs(q1));
    str_sign = ("(+)" if q1>0 else "(-)");
    print(str_sign, str_x_or_lg("Q", abs(q1)));

if br > 0 :  # remaining sum in the partial series is less 
    L = L + 1 / log10(abs(QQ[-1]));     # than the last term

# Pi sum for sanity check
S = M*math.atan(1/Q0);
for q1 in QQ :
    S = S + math.atan(1/q1);

say = ("(brk)\n---\nLem <" if br>0 else "---\nLem");
print(say, L, "\nPi", 4*S);

# notable fraction: 22/7
# https://www.britannica.com/science/pi-mathematics

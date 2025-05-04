
import math
import numpy as np

def binomial_pmf(k, N, q):
    return math.comb(N, k) * q ** k * (1 - q) ** (N - k)
def binomial_cdf(N, q, end, start=0):
    sum = 0
    for i in range(start, end):
        sum += binomial_pmf(i, N, q)
    return sum


for i in range(22):
    print(binomial_pmf(i, 21, 0.3))
print(binomial_cdf(21, 0.3, 21, 11))



'''
q_lst = [0.1, 0.2, 0.3, 0.4, 0.45, 0.5, 0.55, 0.6, 0.7, 0.8, 0.9]
N_lst = [1, 5, 10, 20, 40, 70, 100]
result = np.ndarray((len(q_lst)+1, len(N_lst)+1))
result[0, :] = np.array([0] + N_lst)
result[1:, 0] = np.array(q_lst)

for i, q in enumerate(q_lst):
    for j, N in enumerate(N_lst):
        result[i+1, j+1] = binomial_cdf(N, q, N+1, int(N / 2) + 1)


print("\\begin{tabular}{|" + "c|" * result.shape[1] + "}")

for i in range(result.shape[0]):
    print("\\hline")
    print(f"{result[i, 0]:.3f}", end="")
    for j in range(1, result.shape[1]):
        print(f" & {result[i, j]:.3f}", end="")
    print(" \\\\")
    
print("\\hline")
print("\\end{tabular}")
'''

        

        




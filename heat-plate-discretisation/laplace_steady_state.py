"""
2D Laplace equation on a unit square plate (ME3291, NUS)

Solves  d2T/dx2 + d2T/dy2 = 0  directly for the steady-state temperature
field using a five-point finite difference stencil and Gauss-Seidel iteration.
The result is compared against the steady state of heat_equation_implicit.py.

Boundary conditions (Dirichlet): top edge held at T = 1, all other edges at T = 0.
"""

from tqdm import tqdm

import numpy as np  # for zero matrix
import matplotlib.pyplot as plt

# Grid
nx, ny = 100, 100
lx, ly = 1, 1
M, N = 100, 100
dx, dy = lx/M, ly/N

# Define boundary conditions
boundary_conditions = {
    'top': 1,  # Top boundary held at 1
    'bottom': 0,
    'right side': 0,
    'left side': 0,
}

# Initialize solution array
phi = np.zeros((nx, ny))
pnew = phi.copy()

# Set boundary conditions
phi[0, :] = boundary_conditions['bottom']       # Bottom boundary
phi[-1, :] = boundary_conditions['top']         # Top boundary
phi[:, 0] = boundary_conditions['left side']    # Left side boundary
phi[:, -1] = boundary_conditions['right side']  # Right side boundary

# Gauss-Seidel
b = np.zeros((nx, ny))  # Laplace: right-hand side = 0

tolerance = 1e-10
max_iter = 10000

# L2 difference function
def l2_diff(a, b):
    return np.sqrt(np.sum((a - b)**2))

pbar = tqdm(total=max_iter)
pbar.set_description("it / max_it")

it = 0  # iteration counter
diff = 1.0
tol_hist_gs = []
while (diff > tolerance):
    if it > max_iter:
        print('\nSolution did not converge within the maximum'
              ' number of iterations'
              f'\nLast l2_diff was: {diff:.5e}')
        break

    np.copyto(phi, pnew)

    # We only modify interior nodes. The boundary nodes remain equal to
    # zero and the Dirichlet boundary conditions are therefore automatically
    # enforced.
    for j in range(1, ny-1):
        for i in range(1, nx-1):
            pnew[i, j] = (0.25 * (pnew[i-1, j] + phi[i+1, j] + pnew[i, j-1]
                                  + phi[i, j+1] - b[i, j]*dx**2))

    pnew[0, :] = boundary_conditions['bottom']
    pnew[-1, :] = boundary_conditions['top']
    pnew[:, 0] = boundary_conditions['left side']
    pnew[:, -1] = boundary_conditions['right side']

    diff = l2_diff(pnew, phi)
    tol_hist_gs.append(diff)

    it += 1
    pbar.update(1)

else:
    print(f'\nThe solution converged after {it} iterations')

del(pbar)

x = np.linspace(0, lx, nx)
y = np.linspace(0, ly, ny)
X, Y = np.meshgrid(x, y)

fig, (ax_1, ax_2) = plt.subplots(1, 2, figsize=(16, 5))
# Plot the solution with matplotlib.pyplot.contourf
ax_1.contourf(X, Y, pnew, 20)
ax_2.contourf(X, Y, phi, 20)

# Add labels and titles
ax_1.set_xlabel(r'$x$')
ax_1.set_ylabel(r'$y$')
ax_1.set_title('Laplace solution')

ax_2.set_xlabel(r'$x$')
ax_2.set_ylabel(r'$y$')
ax_2.set_title('Previous iteration')

plt.show()

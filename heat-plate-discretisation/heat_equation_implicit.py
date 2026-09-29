"""
2D transient heat equation on a unit square plate (ME3291, NUS)

Solves  dT/dt = d2T/dx2 + d2T/dy2  with an implicit finite difference
scheme. Each time step is solved iteratively with the Gauss-Seidel method.

Boundary conditions (Dirichlet): top edge held at T = 1, all other edges at T = 0.
Initial condition: T = 0 everywhere inside the plate.
"""

from tqdm import tqdm
import numpy as np
import matplotlib.pyplot as plt

# Grid
nx, ny = 100, 100
lx, ly = 1, 1
dx, dy = lx/(nx-1), ly/(ny-1)

# Time
dt = 0.01
nt = 100   # number of time steps

# Stability parameter (used in implicit scheme)
alpha = dt / dx**2

# Boundary conditions
boundary_conditions = {
    'top': 1,
    'bottom': 0,
    'right side': 0,
    'left side': 0,
}

# Initialize solution
phi = np.zeros((nx, ny))
pnew = phi.copy()

# Apply boundary conditions
phi[0, :] = boundary_conditions['bottom']
phi[-1, :] = boundary_conditions['top']
phi[:, 0] = boundary_conditions['left side']
phi[:, -1] = boundary_conditions['right side']

pnew[:] = phi[:]

# Convergence settings (for Gauss-Seidel at each time step)
tolerance = 1e-6
max_iter = 5000

# L2 difference
def l2_diff(a, b):
    return np.sqrt(np.sum((a - b)**2))

# Time loop
for t in range(nt):
    diff = 1.0
    it = 0

    # Progress bar for each time step
    pbar = tqdm(total=max_iter, desc=f"time step {t+1}/{nt}")

    while diff > tolerance:
        if it > max_iter:
            print(f"\nNo convergence at time step {t}")
            break

        phi_old = pnew.copy()

        # Gauss-Seidel update (IMPLICIT scheme)
        for j in range(1, ny-1):
            for i in range(1, nx-1):
                pnew[i, j] = (
                    phi[i, j] +
                    alpha * (
                        pnew[i-1, j] + pnew[i+1, j] +
                        pnew[i, j-1] + pnew[i, j+1]
                    )
                ) / (1 + 4 * alpha)

        # Reapply boundary conditions
        pnew[0, :] = boundary_conditions['bottom']
        pnew[-1, :] = boundary_conditions['top']
        pnew[:, 0] = boundary_conditions['left side']
        pnew[:, -1] = boundary_conditions['right side']

        diff = l2_diff(pnew, phi_old)

        it += 1
        pbar.update(1)

    pbar.close()

    # Save snapshots at required times
    if t == 1:      # t = 1*dt = 0.01 s
        T_001 = pnew.copy()

    if t == 10:     # t = 10*dt = 0.1 s
        T_01 = pnew.copy()

    # Update solution for next time step
    phi[:] = pnew[:]

print("\nSimulation complete.")

# Plot result
x = np.linspace(0, lx, nx)
y = np.linspace(0, ly, ny)
X, Y = np.meshgrid(x, y)

# Plot results
fig, axes = plt.subplots(1, 3, figsize=(15, 5))

axes[0].contourf(X, Y, T_001, 20)
axes[0].set_title("t = 0.01 s")

axes[1].contourf(X, Y, T_01, 20)
axes[1].set_title("t = 0.1 s")

axes[2].contourf(X, Y, pnew, 20)
axes[2].set_title("Steady state")

for ax in axes:
    ax.set_xlabel("x")
    ax.set_ylabel("y")

plt.suptitle('Temperature distribution (implicit solution)')
plt.show()

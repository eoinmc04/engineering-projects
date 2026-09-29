"""
Traffic shockwave analysis using real vehicle trajectory data (NGSIM)

Loads the NGSIM vehicle trajectory dataset, isolates the vehicles travelling in
one lane, and plots their position against time. Colouring each trajectory by
instantaneous speed makes stop-and-go "shockwaves" visible: bands where
vehicles slow down that travel backwards along the road.

Data: NGSIM (Next Generation Simulation) vehicle trajectories, U.S. DOT.
The CSV must contain the columns Vehicle_ID, Local_X, Local_Y, Global_Time, v_Vel
(distances in feet, speeds in ft/s, time in ms). Place it next to this script
as NGSIM.csv.
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib.colors import LinearSegmentedColormap

FT_TO_M = 0.3048

# ----------------------------------------------------------------------------
# 1. Load the data
# ----------------------------------------------------------------------------
input_file = 'NGSIM.csv'

df = pd.read_csv(input_file)
print(df.head())  # Preview the contents of the data

vehicle_ids = np.unique(df['Vehicle_ID'])

t_min, t_max = np.min(df['Global_Time'].to_numpy()/60000), np.max(df['Global_Time'].to_numpy()/60000)  # time limits (min)
v_min, v_max = np.min(df['v_Vel'].to_numpy()*FT_TO_M), np.max(df['v_Vel'].to_numpy()*FT_TO_M)          # velocity limits (m/s)
y_min, y_max = np.min(df['Local_Y'].to_numpy()*FT_TO_M), np.max(df['Local_Y'].to_numpy()*FT_TO_M)      # position limits (m)

# ----------------------------------------------------------------------------
# 2. Select the vehicles in one lane
# ----------------------------------------------------------------------------
# Lane boundaries (m) estimated through statistical analysis of the full dataset
lane_boundaries = [0, 2.6, 4, 7, 10, 15, 18, 22]

lane_id = 6  # Lane 0-6. Lane 6 showed the clearest shockwave patterns.

veh_id_lane = []
for veh_id in vehicle_ids:
    veh_df = df[df['Vehicle_ID'] == veh_id]
    x = veh_df['Local_X'].to_numpy()*FT_TO_M  # Position across the road

    # Keep the vehicle if it spends most of its recorded time in the chosen lane
    if lane_boundaries[lane_id] < np.average(x) < lane_boundaries[lane_id + 1]:
        veh_id_lane.append(veh_id)


def get_vehicle(veh_id):
    """Return position along the road (m), time (min) and speed (m/s) for one vehicle."""
    veh_df = df[df['Vehicle_ID'] == veh_id]
    y = veh_df['Local_Y'].to_numpy()*FT_TO_M
    t = veh_df['Global_Time'].to_numpy()/60000 - t_min
    v = veh_df['v_Vel'].to_numpy()*FT_TO_M
    return y, t, v


def data_is_valid(veh_id, y, t, v):
    """Sanity checks so bad records don't corrupt the plots."""
    if len(t) == 0 or len(y) == 0 or len(v) == 0:
        print(f"Vehicle {veh_id}: one or more data arrays are empty, skipping")
        return False
    dy = y[1:] - y[:-1]
    if len(dy) and np.min(dy) < -100:
        raise SystemExit(f'ValueError: vehicle {veh_id} is moving backwards in the data')
    return True


# ----------------------------------------------------------------------------
# 3. Trajectory plot: position vs time for every vehicle in the lane
# ----------------------------------------------------------------------------
plt.figure()
for veh_id in veh_id_lane:
    y, t, v = get_vehicle(veh_id)
    if not data_is_valid(veh_id, y, t, v):
        continue
    plt.plot(t, y, linewidth=0.4, color='r', alpha=0.8)

plt.xlabel('time (mins)')
plt.ylabel('position (m)')
plt.title('Trajectory plot')
plt.xlim(0.0, 20.0)
plt.show()
plt.close()

# ----------------------------------------------------------------------------
# 4. Trajectories coloured by speed, zoomed in on the shockwaves
# ----------------------------------------------------------------------------
# Optional custom colour map (brown = slow ... blue = fast)
colors = [(165/255, 42/255, 42/255),    # Brown
          (255/255, 69/255, 0/255),     # OrangeRed
          (255/255, 165/255, 0/255),    # Orange
          (255/255, 255/255, 0/255),    # Yellow
          (34/255, 139/255, 34/255),    # Green
          (64/255, 224/255, 208/255),   # Turquoise
          (135/255, 206/255, 235/255),  # Sky blue
          (0/255, 0/255, 255/255)]      # Blue
custom_cmap = LinearSegmentedColormap.from_list("my_cmap", colors)

plt.figure()
lc = None
for veh_id in veh_id_lane:
    y, t, v = get_vehicle(veh_id)
    if not data_is_valid(veh_id, y, t, v):
        continue

    # Split each trajectory into small segments so each can take its own colour
    points = np.array([t, y/1000]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)

    lc = LineCollection(segments, cmap='coolwarm', linewidth=0.4, norm=plt.Normalize(5, 15))
    lc.set_array(v)
    plt.gca().add_collection(lc)

if lc is not None:
    cbar = plt.colorbar(lc)
    cbar.set_label('velocity (m/s)', rotation=270, labelpad=20, fontsize=15)

plt.xlabel('time (min)', fontsize=15)
plt.ylabel('position (km)', fontsize=15)
plt.tick_params(axis='both', which='major', labelsize=12)
plt.ylim(0, 0.52)

# Time window where the shockwaves are most clearly visible
x_lim_low, x_lim_high = 10, 16
plt.xlim(x_lim_low, x_lim_high)

plt.tight_layout()
plt.show()
plt.close()

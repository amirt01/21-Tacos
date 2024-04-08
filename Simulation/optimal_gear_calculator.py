from math import pi
import matplotlib.pyplot as plt
import numpy as np

# Set Global Parameters
chainring_diameter = 36  # [mm]
cassette = [28, 24, 21, 18, 16, 14]  # [mm]

wheel_diameter = 622  # [mm]
tire_size = 23  # [mm]


# Calculate the speed ratio between the crank [rpm] and the ground [kph]
def calculate_speed_ratio(cog_diameter):
    return pi * (wheel_diameter + (2 * tire_size)) / 1000 * (chainring_diameter / cog_diameter)


def calculate_nominal_speed(cog_diameter, cadence):
    return calculate_speed_ratio(cog_diameter) * cadence / 60 * 3.6


def calculate_speed_dif(speed, cog_diameter, cadence):
    return abs(speed - calculate_nominal_speed(cog_diameter, cadence))


def calculate_optimal_gear(speed, cadence):
    return min(enumerate(cassette, start=1), key=lambda cog: calculate_speed_dif(speed, cog[1], cadence))[0]


def plot_speed_range(min_cadence, max_cadence):
    min_speeds = np.array([calculate_nominal_speed(cog_diameter, min_cadence) for cog_diameter in cassette])
    max_speeds = np.array([calculate_nominal_speed(cog_diameter, max_cadence) for cog_diameter in cassette])
    ind = range(1, len(cassette) + 1)

    plt.barh(ind, np.subtract(max_speeds, min_speeds), 0.35, min_speeds)
    plt.xlim(xmin=plt.xlim()[0] - 1)
    plt.xlabel('Speed (km/h)')
    plt.ylabel('Gears')
    plt.title(f'Speed Range When Pedaling between {min_cadence} and {max_cadence} RPM')
    plt.show()


def main():
    cadence_range = [90, 120]
    plot_speed_range(*cadence_range)

    cadence = 90  # [RPM]
    speed = 20  # [km/h]
    optimal_gear = calculate_optimal_gear(speed, cadence)
    print(f"The optimal gear for a cadence of {cadence} RPM and "
          f"ground speed of {speed} km/h is gear {optimal_gear}.")


if __name__ == "__main__":
    main()

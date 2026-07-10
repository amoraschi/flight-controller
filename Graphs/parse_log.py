import sys
import csv
import matplotlib.pyplot as plt
import numpy as np


def plot_three_axes(filename, title, ylabel, time_s, x, y, z):
    fig, axes = plt.subplots(3, 1, figsize=(12, 9), sharex=True)

    data = [
        ("X", x, "#e74c3c"),
        ("Y", y, "#2ecc71"),
        ("Z", z, "#3498db"),
    ]

    for ax, (axis, values, color) in zip(axes, data):
        ax.plot(time_s, values, color=color, linewidth=0.5)
        ax.set_title(f"{title} {axis}")
        ax.set_ylabel(ylabel)
        ax.ticklabel_format(axis="y", useOffset=False, style="plain")
        ax.grid(True, alpha=0.3)

    axes[-1].set_xlabel("Time (s)")
    fig.tight_layout()
    fig.savefig(filename, dpi=150)
    plt.close(fig)

    print(f"Saved {filename}")


def plot_single(filename, title, ylabel, time_s, values):
    fig, ax = plt.subplots(figsize=(12, 4))

    ax.plot(time_s, values, color="#e74c3c", linewidth=0.5)
    ax.set_title(title)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel(ylabel)
    ax.ticklabel_format(axis="y", useOffset=False, style="plain")
    ax.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(filename, dpi=150)
    plt.close(fig)

    print(f"Saved {filename}")


def parse_log(path):
    with open(path, "r") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    if not rows:
        print("No records found.")
        sys.exit(1)

    print(f"Found {len(rows)} records")

    time_s = np.array([float(r["TimestampMS"]) / 1000.0 for r in rows])

    accel_x = np.array([float(r["AccelX"]) for r in rows])
    accel_y = np.array([float(r["AccelY"]) for r in rows])
    accel_z = np.array([float(r["AccelZ"]) for r in rows])

    gyro_x = np.array([float(r["GyroX"]) for r in rows])
    gyro_y = np.array([float(r["GyroY"]) for r in rows])
    gyro_z = np.array([float(r["GyroZ"]) for r in rows])

    mag_x = np.array([float(r["MagX"]) for r in rows])
    mag_y = np.array([float(r["MagY"]) for r in rows])
    mag_z = np.array([float(r["MagZ"]) for r in rows])

    pressure = np.array([float(r["PressurePa"]) for r in rows])
    temperature = np.array([float(r["TemperatureC"]) for r in rows])
    altitude = np.array([float(r["Altitude"]) for r in rows])
    velocity_z = np.array([float(r["VelocityZ"]) for r in rows])

    # Three-axis plots
    plot_three_axes(
        "accelerometer.png",
        "Accelerometer",
        "Acceleration (m/s²)",
        time_s,
        accel_x,
        accel_y,
        accel_z,
    )

    plot_three_axes(
        "gyroscope.png",
        "Gyroscope",
        "Angular Rate (dps)",
        time_s,
        gyro_x,
        gyro_y,
        gyro_z,
    )

    plot_three_axes(
        "magnetometer.png",
        "Magnetometer",
        "Field (mGauss)",
        time_s,
        mag_x,
        mag_y,
        mag_z,
    )

    # Single-value plots
    plot_single(
        "pressure.png",
        "Pressure",
        "Pressure (Pa)",
        time_s,
        pressure,
    )

    plot_single(
        "temperature.png",
        "Temperature",
        "Temperature (°C)",
        time_s,
        temperature,
    )

    plot_single(
        "altitude.png",
        "Altitude",
        "Altitude (m)",
        time_s,
        altitude,
    )

    plot_single(
        "velocity_z.png",
        "Velocity Z",
        "Velocity (m/s)",
        time_s,
        velocity_z,
    )

    # Combined plot
    fig, axes = plt.subplots(7, 1, figsize=(14, 21), sharex=True)

    combined = [
        ("Accelerometer", "Acceleration (m/s²)", accel_x, accel_y, accel_z),
        ("Gyroscope", "Angular Rate (dps)", gyro_x, gyro_y, gyro_z),
        ("Magnetometer", "Field (mGauss)", mag_x, mag_y, mag_z),
        ("Pressure", "Pressure (Pa)", pressure, None, None),
        ("Temperature", "Temperature (°C)", temperature, None, None),
        ("Altitude", "Altitude (m)", altitude, None, None),
        ("Velocity Z", "Velocity (m/s)", velocity_z, None, None),
    ]

    colors = ["#e74c3c", "#2ecc71", "#3498db"]

    for ax, (title, ylabel, a, b, c) in zip(axes, combined):
        ax.plot(time_s, a, color=colors[0], linewidth=0.5,
                label="X" if b is not None else None)

        if b is not None:
            ax.plot(time_s, b, color=colors[1], linewidth=0.5, label="Y")
            ax.plot(time_s, c, color=colors[2], linewidth=0.5, label="Z")
            ax.legend(loc="upper right", fontsize=8)

        ax.set_title(title)
        ax.set_ylabel(ylabel)
        ax.ticklabel_format(axis="y", useOffset=False, style="plain")
        ax.grid(True, alpha=0.3)

    axes[-1].set_xlabel("Time (s)")
    fig.tight_layout()
    fig.savefig("all_plots.png", dpi=150)
    plt.close(fig)

    print("Saved all_plots.png")

    # plt.show()


if __name__ == "__main__":
    parse_log("decoded_log0.csv")

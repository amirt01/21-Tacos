class ReedSwitch:
    def __init__(self, rpm: float):
        self.rpm = rpm
        self.last_rev_time = time.time()

    def read(self) -> bool:
        spr = float("inf") if self.rpm == 0 else 60 / self.rpm

        passing_time = 5 * spr / 360

        t = time.time()
        time_since_last_rev = t - self.last_rev_time
        if time_since_last_rev < passing_time:
            return True
        if time_since_last_rev < spr:
            return False
        else:
            # rev_time should be at the center of the magnet passing
            self.last_rev_time = t
            return True


def constant_360rpm():
    rs = ReedSwitch(rpm=360)

    times = []
    values = []

    t_start = time.time()
    t_end = t_start + 5
    while time.time() < t_end:
        times.append(time.time() - t_start)
        values.append(rs.read())

    plt.plot(times, values)
    plt.title("Reed Switch Value Over Time at 360rpm")
    plt.xlabel("Time (s)")
    plt.ylabel("Value")
    plt.show()


def ramp_360rpm():
    rs = ReedSwitch(rpm=0)

    target_rpm = 360
    total_time = 5
    acceleration = target_rpm / total_time

    times = []
    values = []

    t_start = time.time()
    t_end = t_start + total_time
    last_update_time = t_start
    while time.time() < t_end:
        dt = time.time() - last_update_time
        last_update_time = time.time()
        rs.rpm += acceleration * dt
        times.append(time.time() - t_start)
        values.append(rs.read())

    plt.plot(times, values)
    plt.title("Reed Switch Value Over Time ramping to 360rpm")
    plt.xlabel("Time (s)")
    plt.ylabel("Value")
    plt.show()


if __name__ == "__main__":
    import time
    import matplotlib.pyplot as plt

    constant_360rpm()
    ramp_360rpm()

import time
import matplotlib.pyplot as plt


class ReedSwitch:
    def __init__(self, rpm: float):
        self.spr = 60 / rpm
        self.passing_time = 5 * self.spr / 360
        self.last_rev_time = time.time()

    def read(self) -> bool:
        t = time.time()
        time_since_last_rev = t - self.last_rev_time
        if time_since_last_rev < self.passing_time:
            return True
        if time_since_last_rev < self.spr:
            return False
        else:
            # rev_time should be at the center of the magnet passing
            self.last_rev_time = t
            return False


if __name__ == "__main__":
    rs = ReedSwitch(rpm=120)
    print("spr: ", rs.spr)
    print("passing_time: ", rs.passing_time)

    times = []
    values = []

    t_start = time.time()
    t_end = t_start + 5
    while time.time() < t_end:
        times.append(time.time() - t_start)
        values.append(rs.read())

    plt.plot(times, values)
    plt.title("Reed Switch Value Over Time at 120rpm")
    plt.xlabel("Time (s)")
    plt.ylabel("Value")
    plt.show()

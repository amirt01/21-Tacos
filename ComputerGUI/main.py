from tkinter import ttk

from ttkthemes import ThemedTk

import requests

URL = "http://192.168.4.1/cadence"

root = ThemedTk(theme="arc", background=True)
root.geometry("300x400")
count_label = ttk.Label(root, text="test")
count_label.pack()
while True:
    r = requests.get(url=URL)
    count = r.text

    count_label.config(text="count: " + count)

    root.update()

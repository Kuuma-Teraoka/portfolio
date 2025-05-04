#!/usr/bin/env python3

help=str(
    "\n"
    "Usage: network MATRIX_FILE OUTPUT_FILE\n"
    "\n"
)

import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import matrix
import sys

if len(sys.argv) != 3:
    print(help)
    exit()

G = nx.from_numpy_array(matrix.matrix(sys.argv[1]))
plt.figure(figsize=(8, 8))
pos = nx.spring_layout(G)
#nx.draw(G, pos, with_labels=True, node_color="lightblue", edge_color="gray", node_size=700, font_size=12)
nx.draw(G, pos, node_color="#0000FF", edge_color="#000000", node_size=700)

plt.savefig(sys.argv[2], format="pdf")
plt.close()


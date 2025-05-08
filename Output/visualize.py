import re
import sys
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

def visualize_nodes(file_path):
    # Parse the node file
    nodes = []
    pattern = re.compile(r'Node ID:\s*(\S+)\s*Coords:\s*\(\s*(\d+)\s*,\s*(\d+)\s*\)')
    with open(file_path, 'r') as f:
        for line in f:
            m = pattern.search(line)
            if m:
                node_id = m.group(1)
                x = int(m.group(2))
                y = int(m.group(3))
                nodes.append((node_id, x, y))

    if not nodes:
        raise ValueError(f"No valid Node entries found in {file_path!r}")

    # Compute grid bounds
    max_x = max(x for _, x, _ in nodes)
    max_y = max(y for _, _, y in nodes)

    # Create plot
    fig, ax = plt.subplots()
    for node_id, x, y in nodes:
        ax.add_patch(Rectangle((x, y), 1, 1, fill=False, edgecolor='black'))
        ax.text(x + 0.5, y + 0.5, node_id, ha='center', va='center')

    # Configure axes
    ax.set_xlim(0, max_x + 1)
    ax.set_ylim(0, max_y + 1)
    ax.set_aspect('equal')
    ax.set_xticks(range(max_x + 1))
    ax.set_yticks(range(max_y + 1))
    ax.grid(True)
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_title('Node Layout Visualizer')
    # plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <nodes_file>")
        sys.exit(1)
    visualize_nodes(sys.argv[1])

    plt.tight_layout()                              # clean up margins
    output_png = "nodes_layout.png"                 # choose your filename
    plt.savefig(output_png, dpi=150)                # write out to PNG
    print(f"Visualization saved as {output_png!r}")

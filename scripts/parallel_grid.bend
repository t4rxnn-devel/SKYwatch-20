# SkyWatch-20 Massively Parallel Simulator (Bend Component)
# Run via terminal using: bend run scripts/parallel_grid.bend

# Defines a simple 2D coordinate container
object Target:
  x
  y

# Parallel divide-and-conquer recursive distance aggregation
def compute_parallel_matrix(depth, target_a, target_b):
  if depth == 0:
    # Base case: calculate absolute spatial coordinate deltas
    dx = target_a.x - target_b.x
    dy = target_a.y - target_b.y
    return (dx * dx) + (dy * dy)
  else:
    # Forking stage: Bend splits these calls across separate GPU threads automatically!
    left_fork  = compute_parallel_matrix(depth - 1, target_a, target_b)
    right_fork = compute_parallel_matrix(depth - 1, target_b, target_a)
    return left_fork + right_fork

def main():
  # Spawn mock target parameters
  plane_1 = Target { x: 12.5, y: 18.2 }
  plane_2 = Target { x: 14.1, y: 19.5 }

  # Execute a deep parallel calculation tree (Depth 4 = 16 simultaneous operations)
  total_variance = compute_parallel_matrix(4, plane_1, plane_2)
  return total_variance

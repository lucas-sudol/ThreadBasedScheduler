import subprocess
import math
import re

def run_parallel(test_file, print_output):
    """
    Runs the parallel A1 program and returns the computed pi value from stdout.
    """
    result = subprocess.run(['./pthreads', test_file, str(print_output)], capture_output=True, text=True)
    
    # Use regex to extract the Pi value from stdout
    match = re.search(r"the result is ([0-9]+\.[0-9]+)", result.stdout)
    
    if match:
        return float(match.group(1))  # Extract the Pi value from the regex match
    else:
        return None

def run_serial(num_iterations):
    """
    Runs the serial version of the pi computation and returns the computed pi value.
    """
    pi_estimate = 0
    for i in range(num_iterations):
        pi_estimate += (-1)**i / (2 * i + 1)
    pi_estimate *= 4
    return pi_estimate

def compare_results(parallel_pi, serial_pi, tolerance=1e-20):
    """
    Compares the parallel and serial pi values and checks if they are within a certain tolerance.
    """
    return math.isclose(parallel_pi, serial_pi, rel_tol=tolerance)

def generate_test_file(filename, num_threads, tasks):
    """
    Generates the test.txt file for the parallel version with the specified number of threads and tasks.
    """
    with open(filename, 'w') as f:
        f.write(f"{num_threads}\n")
        for task in tasks:
            f.write(f"{task}\n")

def main():
    #Ensure repository is compiled
    subprocess.run(['make', 'all'], check=True)

    # Define the four test cases
    test_cases = [
        (1, [10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000]),  # 1 thread, varied iterations
        (8, [10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000]),  # 8 threads, varied iterations
        (4, [10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000]),  # 4 threads, varied iterations
    ]

    for index, (num_threads, iteration_list) in enumerate(test_cases, start=1):
        print(f"Test Case {index} with {num_threads} thread(s):")

        # Loop through each task (iteration count) in the iteration list
        for task_iterations in iteration_list:
            # Generate test file for parallel execution with a single task
            test_file = 'test.txt'
            generate_test_file(test_file, num_threads, [task_iterations])

            # Run the parallel version
            parallel_pi = run_parallel(test_file, print_output=True)
            if parallel_pi is None:
                print(f"  {task_iterations:<12} iterations:\tError in parallel execution, skipping...")
                continue

            # Run the serial version for the same task
            serial_pi = run_serial(task_iterations)

            # Compare the parallel and serial results and output a concise result
            result = "PASS" if compare_results(parallel_pi, serial_pi) else "FAIL"
            print(f"  iterations:\t{task_iterations:<12} \tParallel Pi = {parallel_pi:.10f}\tSerial Pi = {serial_pi:.10f}\t{result}")

if __name__ == '__main__':
    main()

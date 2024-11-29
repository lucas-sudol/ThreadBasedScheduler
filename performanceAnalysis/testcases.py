import subprocess
import csv
import time
import random
import os

def generate_test_file(filename, num_threads, tasks):
    """
    Generates the test.txt file with the specified number of threads and tasks.
    """
    with open(filename, 'w') as f:
        # print(f"{num_threads}")

        f.write(f"{num_threads}\n")
        for task in tasks:
            # print(f"{task}")

            f.write(f"{task}\n")

def run_test(test_file, print_output, repetitions=30):
    """
    Runs the A1 program multiple times, collects timings, and returns the results.
    """
    timings = []
    for _ in range(repetitions):
        start_time = time.time()
        # Run the A1 program (replace './pthreads' with the actual command to run it if needed)
        subprocess.run(['./pthreads', test_file, str(print_output)], check=True)
        end_time = time.time()
        timings.append(end_time - start_time)
    
    return timings

def analyze_data(timings):
    """
    Analyze the timing data and return statistics such as mean, median.
    """
    mean_time = sum(timings) / len(timings)
    median_time = sorted(timings)[len(timings) // 2]
    return mean_time, median_time

def save_data_to_csv(filename, num_threads, tasks, mean_time, median_time, timings):
    """
    Saves the test data to a CSV file.
    """
    with open(filename, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([num_threads, len(tasks), sum(tasks), mean_time, median_time, *timings])

def create_test_cases():
    """
    Creates a variety of test cases to explore different thread and task configurations.
    """
    sys_cores = os.cpu_count()  # Get the number of cores on the system

    iterations = 1000000000  # Total number of iterations for pi calculation
    test_cases = []

    """
    #Case 1 
    iterations = 1000000000 
    for i in range(1, 17):
        #Divide iterations into 1000 tasks
        test_cases.append((i, [iterations // 1000] * 1000))
    
    #Case 2
    iterations = 1000000000 
    for i in range(16, 65):
        #Divide iterations into 1000 tasks
        test_cases.append((i, [iterations // 1000] * 1000))

    #Case 3 
    iterations = 1000000000 
    for i in range(1, 17):
        if(iterations % i != 0):
            array = ([iterations // i] * i)
            array.append(iterations % i)
            test_cases.append((i, array))

        else:
            test_cases.append((i, [iterations // i] * i))

    #Case 4 
    iterations = 1000000000 
    threadCount = 16;
    for i in range(1, 101):
        if(iterations % i != 0):
            array = ([iterations // i] * i)
            array[-1] += (iterations % i) #Dont change task count, but add remainder to one of the tasks
            test_cases.append((threadCount, array))

        else:
            test_cases.append((threadCount, [iterations // i] * i))
    """
    """
    #Case 5
    threadCount = 8;
    taskCount = 8
    for i in range(1, 11):
        iterations = i * 100000000

        if(iterations % taskCount != 0):
            array = ([iterations // taskCount] * taskCount)
            array[-1] += (iterations % taskCount) #Dont change task count, but add remainder to one of the tasks
            test_cases.append((threadCount, array))

        else:
            test_cases.append((threadCount, [iterations // taskCount] * taskCount))

    #Run again for 1 thread
    for i in range(1, 11):
        iterations = i * 100000000
        test_cases.append((1, [iterations] * 1))

    """
    
    return test_cases

def main():
    output_file = 'test_results.csv'

    #Ensure repository is compiled
    subprocess.run(['make', 'all'], check=True)

    runs = 100;

    
    # Write the header of the CSV file
    with open(output_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Threads', 'Tasks', 'Total Iterations', 'Mean Time', 'Median Time', *[f'Timing {i+1}' for i in range(runs)]])
    
    # Generate and run test cases
    test_cases = create_test_cases()
    i = 0

    for num_threads, tasks in test_cases:
        test_file = 'test.txt'

        i += 1
        print("\nTest ", i)

        generate_test_file(test_file, num_threads, tasks)
        
        # Run the test 30 times and collect the data
        timings = run_test(test_file, print_output=False, repetitions=runs)
        mean_time, median_time = analyze_data(timings)
        
        # Save the results to the CSV file
        save_data_to_csv(output_file, num_threads, tasks, mean_time, median_time, timings)
    
    print(f"Test cases completed. Data saved to {output_file}")

if __name__ == '__main__':
    main()
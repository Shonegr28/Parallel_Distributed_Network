import numpy as np
import os
import pandas as pd
import sys

# Tell the script where to find the base autograder
sys.path.append("..")
sys.path.append(os.path.join("..", ".."))
from autograder_base import Base_Autograder

# Colors
W = '\033[0m'  # white (normal)
R = '\033[31m'  # red
O = '\033[33m'  # orange
Y = '\033[93m'  # yellow
G = '\033[32m'  # green


"""
Project 1, Problem 2 Autograder

Autogrades a single students code
NOTE: As this is the only problem that needs to be autograded for Project 1, only this one file is needed
"""
class Autograder_1_2(Base_Autograder):

    """
    Initializes variables
    """
    def __init__(self, in_student_name="student", in_this_dir=".", in_test_files=["..", "test_data"]):
        super().__init__()

        # Student information
        self.student_name = in_student_name
        self.is_grad = True
        self.DEBUG = True

        # Directory information
        self.this_dir =         in_this_dir
        self.student_files =    "Problem_2"
        self.test_files =       ""

        for i in range(len(in_test_files)):
            self.test_files = os.path.join(self.test_files, in_test_files[i])
    
        self.test_files = os.path.join(self.test_files, "Problem_2")

        # Test information
        self.test_names = [
            "P2-1",
            "P2-2",
            "P2-3"
        ]


    """
    Check if the student's answer is within a reasonable bound of the actual answer
    Error Bound:
        - Check that student's answer is within 1% of actual answer

    Parameters:
        - expected  (ndarray):  The actual answer read from test_data/
        - result    (ndarray):  The student's answer
    """
    def is_error_within_bound(self, expected, result):

        try:
            # Make sure the shapes of the 
            if expected.shape != result.shape:
                raise Exception("Shapes of expected output and student output do not match")
            
            # Compare the two arrays
            return np.array_equal(expected, result, equal_nan=True)
        
        except Exception as err:
            print(f"{R}Error reading output file:{W}")
            print(f"{R}\t{err}{W}")

        return


    """
    Autogrades Problem 2
    Overrides Base_Autograder.autograde()

    Constructs a test by retrieving data about paths and data locations, then calls Base_Autograder.grade_problem()
    to test and grade the problem
    """
    def autograde(self):
        this_dir =  os.path.abspath(self.this_dir)
        test_dir =  os.path.abspath(self.test_files)

        # Print the test dir and project dir
        if self.DEBUG:
            print(f"{G} --> Test dir: {test_dir}{W}")
            print(f"{G} --> Project dir: {this_dir}{W}")

        columns = []
        for p in self.test_names:
            columns.append(f"{p}")

        # student grades
        grade = pd.DataFrame(
            np.nan,
            index=[self.student_name],
            columns=columns
        )

        # student timing
        time = pd.DataFrame(
            np.nan,
            index=[self.student_name],
            columns=columns
        )

        # Input matrix files
        t_matrix = [
            os.path.join(test_dir, "test1_input_mat.csv"),
            os.path.join(test_dir, "test2_input_mat.csv"),
            os.path.join(test_dir, "test3_input_mat.csv")
        ]

        # Input vector files
        t_vector = [
            os.path.join(test_dir, "test1_input_vec.csv"),
            os.path.join(test_dir, "test2_input_vec.csv"),
            os.path.join(test_dir, "test3_input_vec.csv")
        ]

        # Expected output files
        t_out = [
            os.path.join(test_dir, "test1_output_vec.csv"),
            os.path.join(test_dir, "test2_output_vec.csv"),
            os.path.join(test_dir, "test3_output_vec.csv")
        ]

        # Actual output from the student
        t_dir = os.path.join(this_dir, self.student_files)
        t_get = []

        for out in range(len(t_out)):
            t_get.append(os.path.join(t_dir, f"test{out}_output_vec.csv"))

        # Generate commands for the program
        # Command structure:
        #   serial_mult_mat_vec file_1.csv n_row_1 n_col_1 file_2.csv n_row_2.csv result_vector.csv
        test_data = [
            [t_matrix[0],   3,   3, t_vector[0],   3],
            [t_matrix[1],  10,  10, t_vector[1],  10],
            [t_matrix[2], 100, 100, t_vector[2], 100]
        ]
        c_p2 = []

        for file in range(len(self.test_names)):
            c_p2.append([
                "serial_mult_mat_vec",
                test_data[file][0],
                test_data[file][1],
                test_data[file][2],
                test_data[file][3],
                test_data[file][4],
                t_get[file]
            ])

        # Add in command "references" that will tell which argument is which (mainly for result csv and time csv)
        # Dictionary format:
        #       key = flag (i.e. "t" = time argument), value = index of argument in command structure
        # TODO: This can be expanded upon later for every argument so no arguments have to be guessed
        c_p2_ref = {"r": 6, "t": -1}

        # Autograde with test parameters
        test_params = []

        for file in range(len(self.test_names)):
            test_params.append(
                [t_dir, t_out[file], t_get[file], c_p2[file], False, self.is_error_within_bound]
            )

        test_results = [None] * len(columns)
        
        # Test every problem
        grade_index = 0
        for file in range(len(self.test_names)):
            params = test_params[file]
            result = self.grade_problem(
                params[0],                      # student directory
                [params[1]],                    # test output
                [params[2]],                    # test results
                [params[3]],                    # commands
                c_p2_ref,                       # command references
                params[4],                      # exact
                params[5]                       # error function to be passed
            )

            test_results[grade_index] = result[0]

            # Add results to dataframes
            grade.loc[self.student_name, columns[grade_index]] = test_results[grade_index][0]
            grade_index += 1

        return [grade, time]
    

"""
Start of Program Logic
"""
def main():
    print(f"{G}Autograding for Project 1 Problem 2:\n{W}")
    
    p2 = Autograder_1_2()
    res = p2.autograde()

    total   = len(res[0].columns)
    correct = int(res[0].sum(axis=1)[0])

    print(f"{Y}\nFinal Grades:{W}")
    res[0].to_csv("P1_2_grades.csv")
    print(res[0])

    print((f"\n --> {correct}/{total} problems correct\n"))


if __name__ == "__main__":
    main()
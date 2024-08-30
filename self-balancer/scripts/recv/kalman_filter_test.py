import numpy as np
import matplotlib.pyplot as plt
from generated.messages.imu import imu_pb2

class KalmanFilter:
    def __init__(self):
        # Initialize matrices
        self.H = np.array([
            [1, 0, 0, 0, 0, 0],
            [0, 1, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0],
        ])

        self.F_raw = [
            [1, 0, 0, "-dt", 0, 0],
            [0, 1, 0, 0, "-dt", 0],
            [0, 0, 1, 0, 0, "-dt"],
            [0, 0, 0, 1, 0, 0],
            [0, 0, 0, 0, 1, 0],
            [0, 0, 0, 0, 0, 1]
        ]

        self.B_raw = [
            ["dt", 0, 0],
            [0, "dt", 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0]
        ]

        self.P = np.array([
            [9999, 0, 0, 0, 0, 0],
            [0, 9999, 0, 0, 0, 0],
            [0, 0, 9999, 0, 0, 0],
            [0, 0, 0, 9999, 0, 0],
            [0, 0, 0, 0, 9999, 0],
            [0, 0, 0, 0, 0, 9999]
        ])

        self.Q_raw = np.array([
            [1, 1, 1, 1, 1, 1]
        ])

        self.R_raw = np.array([
            [1, 0, 0],
            [0, 1, 0],
            [0, 0, 1]
        ])

        self.X = np.array([0, 0, 0, 0, 0, 0])

        # Initialize storage for plotting
        self.state_history = []
        self.measurement_history = []
        # Plot initialization variables
        self.fig, self.ax = None, None
        self.lines = None


    def compute_matrix_with_dt(self, matrix, dt):
        # Replace "-dt" with -dt and "dt" with dt
        matrix_modified = []
        for row in matrix:
            new_row = []
            for element in row:
                if element == "-dt":
                    new_row.append(-dt)
                elif element == "dt":
                    new_row.append(dt)
                else:
                    new_row.append(element)
            matrix_modified.append(new_row)

        # Convert to a numpy array
        matrix_array = np.array(matrix_modified)
        return matrix_array

    def kalman_filter_predict(self, X, F, B, U, P, Q):
        # Predict the state
        X = np.matmul(F, X) + np.matmul(B, U)
        # Predict the error covariance
        P = np.matmul(F, np.matmul(P, F.T)) + Q
        return X, P

    def kalman_filter_update(self, X, P, Z, R, H):
        # Compute the innovation
        Y = Z - np.matmul(H, X)
        # Compute the innovation covariance
        S = np.matmul(H, np.matmul(P, H.T)) + R
        # Compute the Kalman gain
        K = np.matmul(P, np.matmul(H.T, np.linalg.inv(S)))
        # Update the state estimate
        X = X + np.matmul(K, Y)
        # Update the error covariance
        P = np.matmul((np.eye(6) - np.matmul(K, H)), P)
        return X, P

    def run(self, msg: imu_pb2):
        dt = 1 / 1000
        F = self.compute_matrix_with_dt(self.F_raw, dt)
        B = self.compute_matrix_with_dt(self.B_raw, dt)
        # Define the process vector as a 1x6 matrix where the first 3 elements are the gyro measurements
        U = np.array([msg.Gyro.x_dps, msg.Gyro.y_dps, msg.Gyro.z_dps])
        Q = self.Q_raw

        self.X, self.P = self.kalman_filter_predict(self.X, F, B, U, self.P, Q)

        # Compute the euler angles from the accelerometer measurements
        theta_x = np.arctan2(msg.Accel.y_m_per_s_squared,
                             (np.sqrt(msg.Accel.x_m_per_s_squared ** 2 + msg.Accel.z_m_per_s_squared ** 2)))
        theta_y = np.arctan2(-msg.Accel.x_m_per_s_squared, msg.Accel.z_m_per_s_squared)
        theta_z = 0

        # convert to degrees
        theta_x = np.degrees(theta_x)
        theta_y = np.degrees(theta_y)
        theta_z = np.degrees(theta_z)

        Z = np.array([theta_x, theta_y, theta_z])

        self.X, self.P = self.kalman_filter_update(self.X, self.P, Z, self.R_raw, self.H)
        # Store state history for plotting
        self.state_history.append(self.X[:2])  # Store only the first 2 states (Euler angles)
        self.measurement_history.append(Z)
        
    def plot(self):
        if self.fig is None or self.ax is None or self.lines is None:
            # Initialize plot if not already done
            plt.ion()  # Turn on interactive mode
            self.fig, self.ax = plt.subplots()
            self.ax.set_title('Real-Time Kalman Filter State Estimates')
            self.ax.set_xlabel('Time step')
            self.ax.set_ylabel('State values (Euler angles in degrees)')

            # Initialize line objects for three states (Euler angles)
            self.lines = [self.ax.plot([], [], label=f'State {i}')[0] for i in range(3)]
            self.ax.legend()

        # Update plot with new data
        for i, state in enumerate(zip(*self.state_history)):  # Unpack state history for plotting
            self.lines[i].set_data(range(len(state)), state)
            self.ax.set_xlim(0, len(state))
            self.ax.set_ylim(min(state) - 5, max(state) + 5)

        plt.draw()
        plt.pause(0.0001)  # Pause to update the plot in real-time
        print(f"State estimate: {self.X[:2]}")
        

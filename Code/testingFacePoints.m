clc;
close all;

X = [-0.333333, -0.872678, -0.127322, -0.872678, -0.745356, -0.666667, -0.745356, -0.127322, 0.127322, 0.333333, 0.872678, 0.872678, 0.666667, 0.745356, 0.745356, 0.333333, 0.127322, 0, -0.333333, 0]
Y = [0.577350, 0.356822, 0.934172, -0.356822, 0.577350, 0, -0.577350, -0.934172, -0.934172, -0.577350, -0.356822, 0.356822, 0, -0.577350, 0.577350, 0.577350, 0.934172, 0, -0.577350, 0]
Z = [0.745356, 0.333333, -0.333333, 0.333333, -0.333333, -0.745356, -0.333333, -0.333333, 0.333333, -0.745356, -0.333333, -0.333333, 0.745356, 0.333333, 0.333333, -0.745356, 0.333333, 1, 0.745356, -1]

plot3(X,Y,Z,"*");
hold on
sphere();
hold off
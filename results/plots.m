%% TEST ERRORS
COLORS = [0 114/255 189/255; 217/255 83/255 25/255; 237/255 147/255 32/255];
col1 = COLORS(1,:);
col2 = COLORS(2,:);
col3 = COLORS(3,:);
figure(Position=[200,200,1400,1400]); t=tiledlayout(3,2);

% Covertype, Depth 12, Eps 0.001
nexttile;
budget = [0 1 2 3];
standard = [51.78 3.75 3.38 3.37];
chen = [33.61 28.32 28.14 28.14];
treant = [34.84 33.69 33.67 33.67];

plot(budget, standard, "-o", MarkerFaceColor=col1);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=col2);
plot(budget, treant, "-o", MarkerFaceColor=col3);
legend(["Natural","RTrees","Treant"]);
title("Covertype, \epsilon = 0.001");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);

% Covertype N8, Depth 12, Eps 0.001
nexttile;
budget = [0 1 2 3];
standard = [61.86 4.10 3.05 2.81];
chen = [36.70 29.86 29.27 NaN];
treant = [37.70 25.91 25.81 NaN];

plot(budget, standard, "-o", MarkerFaceColor=col1);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=col2);
plot(budget, treant, "-o", MarkerFaceColor=col3);
legend(["Natural","RTrees","Treant"]);
title("Covertype 8-Ensemble, \epsilon = 0.001");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);

% HAR, Depth 8, Eps 0.1
nexttile;
budget = [0 1 2 3 4];
standard = [63.82 4.62 0.20 0.20 0.20];
chen = [52.41 8.28 2.92 2.38 2.31];
treant = [33.74 29.87 29.87 29.87 29.87];

plot(budget, standard, "-o", MarkerFaceColor=col1);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=col2);
plot(budget, treant, "-o", MarkerFaceColor=col3);
legend(["Natural","RTrees","Treant"]);
title("HAR, \epsilon = 0.1");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);

% HAR N8, Depth 8, Eps 0.1
nexttile;
budget = [0 1 2 3 4 5];
standard = [66.46 10.73 0.81 0.14 0.07 0.07];
chen = [57.77 19.48 9.57 3.80 2.04 1.70];
treant = [34.35 29.94 29.94 29.94 29.94 29.94];

plot(budget, standard, "-o", MarkerFaceColor=col1);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=col2);
plot(budget, treant, "-o", MarkerFaceColor=col3);
legend(["Natural","RTrees","Treant"]);
title("HAR 8-Ensemble, \epsilon = 0.1");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);

% MNIST, Depth 24, Eps 0.3
% budget = [0 1 2];
% standard = [88.2 6.0 0.55];
% chen = [87.05 71.69 63.27];
% treant = [35.10 24.63 23.53];
% treantnc = [42.43 35.3 33.47];

% MNIST, Depth 24, Eps 0.1
nexttile;
budget = [0 1 2];
standard = [88.17 9.39 2.07];
chen = [88.46 82.38 NaN];
treantnc = [54.34 50.94 NaN];

plot(budget, standard, "-o", MarkerFaceColor=col1);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=col2);
plot(budget, treantnc, "-o", MarkerFaceColor=col3);
legend(["Natural","RTrees","Treant"]);
title("MNIST, \epsilon = 0.1");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);


% MNIST N8, Depth 24, Eps 0.1
nexttile;
budget = [0 1 2];
standard = [94.24 66.13 29.1];
chen = [92.30 87.07 85.58];
treant = [78.47 52.98 48.36];

plot(budget, standard, "-o", MarkerFaceColor=col1);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=col2);
plot(budget, treant, "-o", MarkerFaceColor=col3);
legend(["Natural","RTrees","Treant"]);
title("MNIST 8-Ensemble, \epsilon = 0.1");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);

%title(t, "Test Accuracy Vs. Attacks");
print("test-scores.eps", "-dpsc");

%% TRAIN TIMES
COLORS = [0 114/255 189/255; 217/255 83/255 25/255; 237/255 177/255 32/255];

figure(Position=[200,200,1400,1400]); t=tiledlayout(3,2);
% Covertype Depth 12, Eps 0.001
nexttile;
times = duration(["00:02:13", "00:03:57", "00:04:54"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = COLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("Covertype, \epsilon = 0.001");

% Covertype N8, Depth 12, Eps 0.001
% standard, chen, robust, robustnc
% [0h 0m 52.18s, 0h 1m 11.18s, 0h 1m 27.00s, 0h 1m 12.72s]
nexttile;
times = duration(["00:00:52", "00:01:11", "00:01:27"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = COLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("Covertype 8-Ensemble, \epsilon = 0.001");

% HAR, Depth 8, Eps 0.1
% ["0h 0m 24.40s", "0h 0m 57.33s", "0h 7m 35.09s"]

nexttile;
times = duration(["00:00:24", "00:00:57", "00:07:35"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = COLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("HAR, \epsilon = 0.1");

% HAR N8, Depth 8, Eps 0.1
% standard, chen, robust, robustnc
% [0h 0m 27.23s, 0h 1m 17.87s, 0h 13m 47.15s, 0h 7m 29.62s]
nexttile;
times = duration(["00:00:27", "00:01:18", "00:13:47"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = COLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("HAR 8-Ensemble, \epsilon = 0.1");


%  MNIST, Depth 24, Eps 0.1
% ["2h 7m 23.72s", "4h 21m 24.57s", "42h 46m 41.14s"]
nexttile;
times = duration(["02:07:24", "04:21:25", "42:46:41"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = COLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("MNIST, \epsilon = 0.1");

% MNIST N8, Depth 24, Eps 0.1
% ["1h 7m 15.33s", "1h 33m 21.69s", "27h 10m 58.73s"]
nexttile;
times = duration(["01:07:15", "01:33:22", "27:10:59"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = COLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("MNIST 8-Ensemble, \epsilon = 0.1");

%title(t, "Training Times");
print("train-times.eps", "-dpsc");
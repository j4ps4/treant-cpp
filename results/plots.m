% TEST ERRORS
%% Covertype, Depth 12, Eps 0.001
budget = [0 1 2 3];
standard = [51.78 3.75 3.38 3.37];
chen = [33.61 28.32 28.14 28.14];
treant = [34.84 33.69 33.67 33.67];

figure;
plot(budget, standard, "-o", MarkerFaceColor=[0.2 0.2 0.8]);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=[0.8 0.2 0.2]);
plot(budget, treant, "-o", MarkerFaceColor=[0.4 0.4 0.2]);
legend(["Natural","RTrees","Treant"]);
title("Covertype, \epsilon = 0.001");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);
print("covertype-test.png", "-dpng");

%% HAR, Depth 8, Eps 0.1
budget = [0 1 2 3 4];
standard = [63.82 4.62 0.20 0.20 0.20];
chen = [52.41 8.28 2.92 2.38 2.31];
treant = [33.74 29.87 29.87 29.87 29.87];

figure;
plot(budget, standard, "-o", MarkerFaceColor=[0.2 0.2 0.8]);
hold on;
plot(budget, chen, "-o", MarkerFaceColor=[0.8 0.2 0.2]);
plot(budget, treant, "-o", MarkerFaceColor=[0.4 0.4 0.2]);
legend(["Natural","RTrees","Treant"]);
title("HAR, \epsilon = 0.1");
xlabel("attacker budget");
ylabel("test accuracy (%)");
xticks(budget);
print("har-test.png", "-dpng");

%% MNIST, Depth 24, Eps 0.3
budget = [0 1 2];
standard = [88.2 6.0 0.55];
chen = [87.05 71.69 63.27];
treant = [35.10 24.63 23.53];
treantnc = [42.43 35.3 33.47];

%% MNIST, Depth 24, Eps 0.1
standard = [88.17];
chen = [88.46];
treantnc = [54.34];


%% MNIST N8, Depth 24, Eps 0.1
standard = [94.24];
chen = [92.30];
treant = [78.47];



% TRAIN TIMES

%% HAR, Depth 8, Eps 0.1
% ["0h 0m 24.40s", "0h 0m 57.33s", "0h 7m 35.09s"]
BARCOLORS = [0 114/255 189/255; 217/255 83/255 25/255; 237/255 177/255 32/255];
figure;
times = duration(["00:00:24", "00:00:57", "00:07:35"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = BARCOLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("HAR, \epsilon = 0.1");

%%  MNIST, Depth 24, Eps 0.1
% ["2h 7m 23.72s", "4h 21m 24.57s", "42h 46m 41.14s"]
figure;
times = duration(["02:07:24", "04:21:25", "42:46:41"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = BARCOLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("MNIST, \epsilon = 0.1");

%% MNIST N8, Depth 24, Eps 0.1
% ["1h 7m 15.33s", "1h 33m 21.69s", "27h 10m 58.73s"]
figure;
times = duration(["01:07:15", "01:33:22", "27:10:59"]);
b=bar(times, FaceColor='flat', EdgeColor=[0.3 0.3 0.3], LineWidth=1.5);
for k =1:3
    b.CData(k,:) = BARCOLORS(k,:);
end
ytickformat('hh:mm');
ylabel("training time (hh:mm)");
xticklabels(["Natural", "RTrees", "Treant"]);
title("MNIST Ensemble, \epsilon = 0.1");
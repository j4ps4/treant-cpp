function run_blackbox(index,model)
    global MNIST;
    orig = MNIST(:,:,index+1);
    orig = imresize(orig, 6);
    [err,true,pred_orig,prob_orig] = run_classify(index,model);
    if err
        return;
    end
    if true ~= pred_orig
        disp("Misclassification.");
        return;
    end
    cmd = sprintf("mpirun -q -np 1 ./treant --blackbox %d -M data/mnist/models/%s -V 5 --n-inst 10000 2>/dev/null", index, model);
    [status, out] = system(cmd);
    if status ~= 0
        fprintf("error: %s\n", out);
        return
    end
    [tokens,~] = regexp(out,"true label = (\d+), predicted label = (\d+), propability = ((\d|\.)+), distortion = ((\d|\.)+), deformed = (.*)",'tokens');
    true_y = eval(tokens{1}{1});
    predicted = eval(tokens{1}{2});
    prob = eval(tokens{1}{3});
    figure(Position=[1000 1000 800 450]);
    subplot(1,2,1);
    imshow(orig, []);
    title(sprintf("true label = %d\n confidence = %.2f%%", true, 100*prob_orig));
    subplot(1,2,2);
    img = eval(tokens{1}{5});
    img = permute(reshape(img, 28, 28), [2 1]);
    img = imresize(img, 6);
    imshow(img, []);
    title(sprintf("predicted label = %d\n confidence = %.2f%%", predicted, 100*prob));
    
    function [err,true,pred,prob] = run_classify(index,model)
        cmd = sprintf("mpirun -q -np 1 ./treant --classify %d -M data/mnist/models/%s 2>/dev/null", index, model);
        [status, out] = system(cmd);
        if status ~= 0
            fprintf("classification error: %s\n", out);
            err = 1; true = 0; pred = 0; prob = 0;
            return;
        end
        [tokens,~] = regexp(out,"true label = (\d+), predicted label = (\d+), propability = ((\d|\.)+)",'tokens');
        true = eval(tokens{1}{1});
        pred = eval(tokens{1}{2});
        prob = eval(tokens{1}{3});
        err = 0;
        return;
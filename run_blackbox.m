function run_blackbox(index)
    global MNIST;
    orig = MNIST(:,:,index+1);
    orig = imresize(orig, 6);
    models = ["mnist-standard.forest", "mnist-chen0.1.forest", "mnist-robust0.1.forest"];
    model_abbr = ["Natural", "RTrees", "Treant"];
    
    for model = models
        [err,true,pred_orig,~] = run_classify(index,model);
        if err
            return;
        end
        if true ~= pred_orig
            fprintf("misclassification for %s\n", model);
            return;
        end
    end

    figure(Position=[1000 1000 1600 450]);
    subplot(1,4,1);
    imshow(orig, []);
    [err,true,~,prob_orig] = run_classify(index,models(1));
    if err
        return;
    end
    
    title(sprintf("label = %d\n confidence = %.2f%%", true, 100*prob_orig));
    model_ix = 1;
    for model = models
        cmd = sprintf("mpirun -q -np 1 ./treant --blackbox %d -M data/mnist/models/%s -V 5 -B 10000 2>/dev/null", index, model);
        [status, out] = system(cmd);
        if status ~= 0
            fprintf("error: %s\n", out);
            return
        end
        [tokens,~] = regexp(out,"true label = (\d+), predicted label = (\d+), propability = ((\d|\.)+), distortion = ((\d|\.)+), deformed = (.*)",'tokens');
        true_y = eval(tokens{1}{1});
        predicted = eval(tokens{1}{2});
        prob = eval(tokens{1}{3});
        deformed = eval(tokens{1}{5});
        deformed = permute(reshape(deformed, 28, 28), [2 1]);
        deformed = imresize(deformed, 6);
        
        subplot(1,4,model_ix+1);

        imshow(deformed, []);
        title(sprintf("label = %d\n confidence = %.2f%%", predicted, 100*prob));
        xlabel(model_abbr(model_ix));

        model_ix = model_ix + 1;
    end
    print(sprintf("sample%d.png", index), "-dpng");
    
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
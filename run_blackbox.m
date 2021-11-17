function run_blackbox(index)
    global MNIST;
    orig = MNIST(:,:,index+1);
    orig = imresize(orig, 6);
    cmd = sprintf("mpirun -q -np 1 ./treant --blackbox %d -M data/mnist/models/mnist-eps0.1.forest 2>/dev/null", index);
    [status, out] = system(cmd);
    if status ~= 0
        fprintf("error: %s\n", out);
        return
    end
    [tokens,matches] = regexp(out,"true label = (\d+), predicted label = (\d+), propability = ((\d|\.)+), distortion = ((\d|\.)+), deformed = (.*)",'tokens','match');
    true_y = eval(tokens{1}{1});
    predicted = eval(tokens{1}{2});
    prob = eval(tokens{1}{3});
    subplot(1,2,1);
    imshow(orig, []);
    title(sprintf("true label = %d", true_y));
    subplot(1,2,2);
    img = eval(tokens{1}{5});
    img = permute(reshape(img, 28, 28), [2 1]);
    img = imresize(img, 6);
    imshow(img, []);
    title(sprintf("predicted label = %d,\n propability = %f", predicted, prob));
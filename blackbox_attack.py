#!/usr/bin/env python3
import time
import random 
import sys
import numpy as np
import torch 
import torch.nn as nn
# import torchvision.datasets as dsets
# import torchvision.transforms as transforms
from torch.autograd import Variable
import torch.nn.functional as F
from models import Treant, MNIST, load_mnist_data, load_har_data, show_image


def attack_untargeted(model, train_dataset, x0, y0, alpha = 0.2, beta = 0.001, iterations = 1000):
    """ Attack the original image and return adversarial example
        model: (pytorch model)
        train_dataset: set of training data
        (x0, y0): original image
    """

    if (model.predict(x0) != y0):
        print("Fail to classify the image. No need to attack.")
        return x0

    num_samples = 1000
    best_theta, g_theta = None, float('inf')
    query_count = 0

    print("Searching for the initial direction on %d samples: " % (num_samples))
    timestart = time.time()
    #samples = set(random.sample(range(len(train_dataset)), num_samples))
    samples =[14337, 57345, 4, 57349, 10250, 55309, 15, 8207, 40977, 2067, 43034, 57371, 4123, 4127, 12320, 16417, 24613, 18470, 55340, 16429, 22573, 43056, 38966, 51256, 57401, 16445, 47166, 12354, 45124, 6214, 74, 2127, 34902, 55383, 4184, 43097, 4186, 14426, 36956, 10334, 30816, 99, 24680, 12394, 55405, 30830, 22639, 2160, 14448, 41074, 55410, 57460, 59509, 26743, 59514, 12411, 51322, 53376, 10368, 36994, 45184, 34945, 14472, 8330, 30860, 51342, 20625, 14487, 8344, 154, 2203, 59549, 32926, 49311, 32927, 41119, 39075, 39078, 51367, 16553, 55467, 41132, 57518, 39091, 43189, 39098, 12476, 30908, 51391, 22719, 14529, 35009, 12481, 12483, 24774, 39111, 4300, 211, 22744, 45273, 4314, 30942, 53471, 222, 57567, 20702, 59621, 37094, 12521, 59628, 33006, 26863, 24815, 20720, 20722, 59638, 51448, 47352, 10490, 4348, 2305, 43269, 24838, 6408, 14602, 22798, 35089, 2324, 51477, 45336, 6427, 14620, 10233, 2337, 10530, 12579, 16676, 20777, 39210, 53550, 49454, 8496, 31022, 2354, 33075, 49463, 53560, 28987, 12603, 39230, 49470, 26944, 37189, 41285, 33095, 6472, 37192, 2384, 41297, 8530, 29012, 8534, 31064, 55648, 16736, 6501, 49511, 8552, 8553, 59754, 53616, 37234, 6514, 55668, 10613, 55671, 20857, 49532, 18814, 43397, 57755, 14751, 39330, 18851, 6565, 16808, 16809, 53674, 22956, 2478, 4527, 37297, 18866, 49585, 35253, 55736, 37306, 59835, 47549, 6593, 31169, 16835, 33217, 53699, 2502, 6599, 43463, 41419, 8652, 51661, 29135, 12752, 33233, 6613, 470, 25049, 8666, 37340, 41439, 51679, 8673, 29151, 2532, 51684, 39400, 12777, 29162, 4587, 55786, 10734, 20975, 55791, 49650, 10738, 59892, 2550, 8696, 6650, 6651, 37372, 31229, 41469, 4609, 31234, 29186, 514, 49666, 33288, 49673, 31240, 4621, 41490, 2580, 39446, 27159, 27158, 2583, 39450, 16922, 31261, 18974, 51744, 33314, 51748, 4644, 47654, 39463, 8743, 18983, 25127, 23083, 27179, 18993, 33331, 25144, 2620, 41533, 23101, 6722, 39491, 29252, 57925, 27206, 583, 16968, 49735, 27208, 53835, 23119, 35408, 19027, 55892, 25176, 41563, 16988, 2653, 29276, 59999, 37473, 31330, 21091, 35428, 43621, 2662, 27239, 2664, 10857, 39530, 4716, 53869, 17017, 57978, 636, 19073, 51841, 25219, 19076, 8836, 19074, 25224, 47753, 17034, 57995, 2700, 21132, 653, 41615, 55952, 23186, 17043, 41619, 51861, 17048, 45722, 55963, 45724, 53917, 39582, 25247, 53918, 58016, 43680, 10915, 31397, 10917, 29358, 17071, 31407, 12974, 12978, 27312, 29368, 47805, 4797, 2752, 27329, 23236, 2759, 6857, 15050, 17099, 41675, 6864, 13009, 45777, 29395, 2772, 6865, 47830, 45782, 51926, 29398, 39639, 45789, 4832, 27360, 33506, 37608, 37609, 19177, 21227, 4849, 47859, 6900, 35573, 56054, 27383, 27385, 25341, 35585, 39682, 47875, 15105, 45830, 21254, 13064, 43788, 27408, 41744, 17170, 21266, 19219, 51991, 45848, 19228, 49949, 54057, 41772, 31534, 815, 54068, 19254, 39736, 31544, 45880, 25404, 21312, 45889, 47938, 54084, 58181, 23367, 4937, 47946, 52045, 35667, 25429, 29528, 35672, 31577, 19292, 37725, 27487, 11103, 865, 54112, 56166, 45928, 52073, 41835, 47979, 2925, 23406, 37745, 21361, 9073, 31602, 27511, 25465, 23420, 25470, 39808, 58246, 43913, 45962, 39819, 5004, 41869, 27532, 2964, 27540, 45974, 920, 29596, 15260, 13217, 11172, 9127, 936, 23465, 27562, 45995, 2994, 54197, 46006, 13237, 21429, 39869, 46017, 965, 9158, 23495, 23498, 15307, 7120, 58324, 3031, 19421, 19423, 41952, 35808, 58343, 1001, 7147, 41970, 56306, 23540, 5106, 29686, 23544, 3065, 48121, 54267, 13317, 3082, 27659, 13330, 9234, 27668, 35863, 48152, 21529, 42008, 39961, 42007, 5147, 46113, 56353, 52260, 35877, 42021, 1062, 25638, 25642, 25643, 39984, 39985, 44082, 54322, 33851, 29756, 5182, 52289, 58435, 52293, 56389, 15433, 19531, 27724, 52301, 58447, 54352, 9298, 58454, 13400, 27737, 3162, 25691, 40035, 44132, 31845, 37990, 23652, 56424, 35945, 7274, 9324, 44141, 35950, 13421, 50288, 54385, 31861, 54390, 35959, 9338, 44155, 44156, 54400, 56450, 46214, 21639, 31879, 5259, 54412, 46221, 46224, 23700, 35988, 15509, 5272, 29849, 19610, 33947, 40093, 46241, 21665, 52385, 19620, 40101, 52389, 52391, 56485, 5290, 13483, 9390, 5295, 40112, 38066, 58552, 23739, 31933, 11456, 52417, 1217, 3267, 13507, 7365, 52425, 25808, 5331, 46292, 34013, 40158, 40159, 46305, 5349, 42214, 31975, 19690, 27893, 58615, 52471, 52477, 44286, 36095, 56578, 17669, 7434, 27916, 13581, 25873, 19730, 56595, 15637, 5398, 27926, 32022, 42266, 44315, 46365, 36127, 58657, 21793, 34082, 27942, 21802, 19755, 15659, 1325, 21807, 48432, 27953, 56626, 13621, 30012, 38204, 1342, 13630, 32065, 1346, 40260, 13639, 21832, 19785, 15690, 44362, 9547, 34135, 38232, 21850, 30042, 38237, 5470, 15709, 17762, 54626, 56674, 7529, 5482, 19819, 30059, 19820, 34159, 7537, 36210, 54646, 34167, 9591, 15737, 7543, 17784, 19835, 30073, 46465, 21890, 5507, 56708, 19844, 32133, 7559, 1417, 11659, 25997, 34196, 1431, 21915, 42396, 52637, 36254, 9631, 9628, 48545, 7587, 11684, 19877, 23973, 5541, 52648, 48554, 5547, 5548, 50605, 7602, 44467, 7605, 44470, 38331, 44480, 50625, 21955, 21959, 58830, 38353, 46547, 21972, 1493, 48599, 46553, 26080, 34278, 54759, 13800, 44521, 44518, 44519, 24044, 32232, 58862, 52716, 44530, 30194, 11769, 15866, 9723, 7676, 54781, 22014, 9725, 24064, 7681, 40461, 44559, 34321, 52758, 50712, 26137, 58906, 56859, 9753, 52763, 32286, 38429, 42528, 42532, 58917, 20008, 11817, 15914, 42550, 15927, 3639, 44603, 9792, 5696, 9800, 34378, 38474, 22090, 40525, 38477, 24138, 1615, 18003, 44629, 32342, 44630, 32347, 28251, 46685, 52829, 56930, 54883, 28260, 22119, 44648, 58988, 34413, 38510, 56943, 15984, 54896, 7792, 18036, 54901, 36469, 54910, 22142, 24191, 42629, 18063, 11925, 32406, 22167, 42649, 34459, 54940, 42652, 32412, 52891, 59041, 7844, 11942, 13993, 9897, 59052, 59053, 46767, 59057, 44730, 9917, 50878, 9920, 24257, 50883, 20169, 52938, 20177, 32466, 1746, 1749, 57047, 40667, 3804, 26333, 40671, 46819, 59108, 48875, 36592, 50928, 59124, 44793, 26362, 38653, 5886, 59133, 40702, 50951, 24334, 34575, 55056, 12050, 1810, 57108, 7954, 12054, 14103, 59160, 44825, 22296, 38684, 53021, 7966, 24351, 12061, 32546, 28453, 57126, 1830, 59179, 46898, 57138, 5940, 50996, 26422, 48953, 48957, 40765, 24383, 34625, 55105, 32577, 14148, 18245, 10054, 14154, 36687, 42832, 38736, 24408, 5977, 12122, 24409, 53086, 30558, 57182, 20325, 3942, 53098, 32619, 53100, 5998, 20335, 53103, 6005, 28537, 49019, 16256, 36739, 1925, 59270, 30598, 51079, 10121, 51078, 34699, 8073, 36754, 30613, 40856, 32665, 47001, 30617, 49052, 14237, 4000, 22433, 28578, 4003, 47010, 55203, 34721, 57255, 4007, 26537, 32679, 55211, 18348, 1974, 8119, 36792, 44985, 20410, 42949, 53190, 49095, 12237, 38864, 8147, 47062, 47063, 10201, 8155, 2012, 57310, 12254, 6112, 57313, 59361, 20451, 45030, 18407, 26601, 10217, 14315, 4075, 57324, 6126, 47087, 4080, 4081, 8179, 57331, 53236, 42997, 49143, 22521, 53242, 6141] 

    for i, (xi, yi) in enumerate(train_dataset):
        #xi = train_dataset[0][i]
        if i not in samples:
            continue
        query_count += 1
        if model.predict(xi) != y0:
            theta = xi - x0
            initial_lbd = torch.norm(theta)
            theta = theta/torch.norm(theta)
            lbd, count = fine_grained_binary_search(model, x0, y0, theta, initial_lbd, g_theta)
            query_count += count
            if lbd < g_theta:
                best_theta, g_theta = theta, lbd
                print("--------> Found distortion %.4f" % g_theta)

    timeend = time.time()
    print("==========> Found best distortion %.4f in %.4f seconds using %d queries" % (g_theta, timeend-timestart, query_count))

    
    
    timestart = time.time()
    g1 = 1.0
    theta, g2 = best_theta.clone(), g_theta
    torch.manual_seed(0)
    opt_count = 0
    stopping = 0.01
    prev_obj = 100000
    for i in range(iterations):
        gradient = torch.zeros(theta.size())
        q = 10
        min_g1 = float('inf')
        for _ in range(q):
            u = torch.randn(theta.size()).type(torch.FloatTensor)
            u = u/torch.norm(u)
            ttt = theta+beta * u
            ttt = ttt/torch.norm(ttt)
            g1, count = fine_grained_binary_search_local(model, x0, y0, ttt, initial_lbd = g2, tol=beta/500)
            opt_count += count
            gradient += (g1-g2)/beta * u
            if g1 < min_g1:
                min_g1 = g1
                min_ttt = ttt
        gradient = 1.0/q * gradient

        if (i+1)%50 == 0:
            print("Iteration %3d: g(theta + beta*u) = %.4f g(theta) = %.4f distortion %.4f num_queries %d" % (i+1, g1, g2, torch.norm(g2*theta), opt_count))
            if g2 > prev_obj-stopping:
                break
            prev_obj = g2

        min_theta = theta
        min_g2 = g2
    
        for _ in range(15):
            new_theta = theta - alpha * gradient
            new_theta = new_theta/torch.norm(new_theta)
            new_g2, count = fine_grained_binary_search_local(model, x0, y0, new_theta, initial_lbd = min_g2, tol=beta/500)
            opt_count += count
            alpha = alpha * 2
            if new_g2 < min_g2:
                min_theta = new_theta 
                min_g2 = new_g2
            else:
                break

        if min_g2 >= g2:
            for _ in range(15):
                alpha = alpha * 0.25
                new_theta = theta - alpha * gradient
                new_theta = new_theta/torch.norm(new_theta)
                new_g2, count = fine_grained_binary_search_local(model, x0, y0, new_theta, initial_lbd = min_g2, tol=beta/500)
                opt_count += count
                if new_g2 < g2:
                    min_theta = new_theta 
                    min_g2 = new_g2
                    break

        if min_g2 <= min_g1:
            theta, g2 = min_theta, min_g2
        else:
            theta, g2 = min_ttt, min_g1

        if g2 < g_theta:
            best_theta, g_theta = theta.clone(), g2
        
        #print(alpha)
        if alpha < 1e-4:
            alpha = 1.0
            print("Warning: not moving, g2 %lf gtheta %lf" % (g2, g_theta))
            beta = beta * 0.1
            if (beta < 0.0005):
                break

    target = model.predict(x0 + g_theta*best_theta)
    timeend = time.time()
    print("\nAdversarial Example Found Successfully: distortion %.4f target %d queries %d \nTime: %.4f seconds" % (g_theta, target, query_count + opt_count, timeend-timestart))
    return x0 + g_theta*best_theta

def fine_grained_binary_search_local(model, x0, y0, theta, initial_lbd = 1.0, tol=1e-5):
    nquery = 0
    lbd = initial_lbd
     
    if model.predict(x0+lbd*theta) == y0:
        lbd_lo = lbd
        lbd_hi = lbd*1.01
        nquery += 1
        while model.predict(x0+lbd_hi*theta) == y0:
            lbd_hi = lbd_hi*1.01
            nquery += 1
            if lbd_hi > 20:
                return float('inf'), nquery
    else:
        lbd_hi = lbd
        lbd_lo = lbd*0.99
        nquery += 1
        while model.predict(x0+lbd_lo*theta) != y0 :
            lbd_lo = lbd_lo*0.99
            nquery += 1

    while (lbd_hi - lbd_lo) > tol:
        lbd_mid = (lbd_lo + lbd_hi)/2.0
        nquery += 1
        if model.predict(x0 + lbd_mid*theta) != y0:
            lbd_hi = lbd_mid
        else:
            lbd_lo = lbd_mid
    return lbd_hi, nquery

def fine_grained_binary_search(model, x0, y0, theta, initial_lbd, current_best):
    nquery = 0
    if initial_lbd > current_best: 
        if model.predict(x0+current_best*theta) == y0:
            nquery += 1
            return float('inf'), nquery
        lbd = current_best
    else:
        lbd = initial_lbd
    
    ## original version
    #lbd = initial_lbd
    #while model.predict(x0 + lbd*theta) == y0:
    #    lbd *= 2
    #    nquery += 1
    #    if lbd > 100:
    #        return float('inf'), nquery
    
    #num_intervals = 100

    # lambdas = np.linspace(0.0, lbd, num_intervals)[1:]
    # lbd_hi = lbd
    # lbd_hi_index = 0
    # for i, lbd in enumerate(lambdas):
    #     nquery += 1
    #     if model.predict(x0 + lbd*theta) != y0:
    #         lbd_hi = lbd
    #         lbd_hi_index = i
    #         break

    # lbd_lo = lambdas[lbd_hi_index - 1]
    lbd_hi = lbd
    lbd_lo = 0.0

    while (lbd_hi - lbd_lo) > 1e-5:
        lbd_mid = (lbd_lo + lbd_hi)/2.0
        nquery += 1
        if model.predict(x0 + lbd_mid*theta) != y0:
            lbd_hi = lbd_mid
        else:
            lbd_lo = lbd_mid
    return lbd_hi, nquery

def attack_mnist(model_name, index, alpha=0.2, beta=0.001, isTarget= False):
    train_loader, test_loader, train_dataset, test_dataset = load_mnist_data()
    print("Length of test_set: ", len(test_dataset))
    dataset = train_dataset
    outfile=open("mnist_attacks.txt","w")

    model = MNIST(model=model_name)
    # if torch.cuda.is_available():
    #     net.cuda()
    #     net = torch.nn.DataParallel(net, device_ids=[0])
        
    #load_model(net, 'models/mnist_gpu.pt')
    #load_model(net, 'models/mnist_cpu.pt')
    #net.eval()

    #model = net.module if torch.cuda.is_available() else net

    def single_attack(image, label, outfile, target = None):
        #show_image(image.numpy())
        print("Original label: ", label)
        print("Predicted label: ", model.predict(image))
        if target == None:
            adversarial = attack_untargeted(model, dataset, image, label, alpha = alpha, beta = beta, iterations = 50)
        else:
            print("Targeted attack: %d" % target)
            adversarial = attack_targeted(model, dataset, image, label, target, alpha = alpha, beta = beta, iterations = 1000)
        show_image(adversarial.numpy())
        prediction=model.predict(adversarial)
        print("true label: {}, predicted label: {}".format(label, prediction))
        # if label == model.predict(image):
        #     outfile.write("\noriginal X: "+str(image))
        #     outfile.write("\nadversarial X: "+str(adversarial))
        #     outfile.write("\nY: "+str(prediction))
        # print("Predicted label for adversarial example: ", prediction)
        return torch.norm(adversarial - image)

    print("Running {} attack on MNIST test image #{} for alpha={} beta={}".format("targetted" if isTarget else "untargeted", index, alpha, beta))
    total_distortion = 0.0

    # samples = [6312, 6891, 4243, 8377, 7962, 6635, 4970, 7809, 5867, 9559, 3579, 8269, 2282, 4618, 2290, 1554, 4105, 9862, 2408, 5082, 1619, 1209, 5410, 7736, 9172, 1650, 5181, 3351, 9053, 7816, 7254, 8542, 4268, 1021, 8990, 231, 1529, 6535, 19, 8087, 5459, 3997, 5329, 1032, 3131, 9299, 3910, 2335, 8897, 7340, 1495, 5244,8323, 8017, 1787, 4939, 9032, 4770, 2045, 8970, 5452, 8853, 3330, 9883, 8966, 9628, 4713, 7291, 9770, 6307, 5195, 9432, 3967, 4757, 3013, 3103, 3060, 541, 4261, 7808, 1132, 1472, 2134, 634, 1315, 8858, 6411, 8595, 4516, 8550, 3859, 3526]
    #true_labels = [3, 1, 6, 6, 9, 2, 7, 5, 5, 3, 3, 4, 5, 6, 7, 9, 1, 6, 3, 4, 0, 6, 5, 9, 7, 0, 3, 1, 6, 6, 9, 6, 4, 7, 6, 3, 4, 3, 4, 3, 0, 7, 3, 5, 3, 9, 3, 1, 9, 1, 3, 0, 2, 9, 9, 2, 2, 3, 3, 3, 0, 5, 2, 5, 2, 7, 2, 2, 5, 7, 4, 9, 9, 0, 0, 7, 9, 4, 5, 5, 2, 3, 5, 9, 3, 0, 9, 0, 1, 2, 9, 9]
    #idx = random.randint(100, len(test_dataset)-1)
    image, label = test_dataset[index]
    print("\n======== Image %d =========" % index)
    #target = None if not isTarget else random.choice(list(range(label)) + list(range(label+1, 10)))
    target = None if not isTarget else (1+label) % 10
    total_distortion += single_attack(image, label, outfile, target)
    
    # print("Average distortion on random {} images is {}".format(num_attacks, total_distortion/num_attacks))

def attack_har(alpha=0.2, beta=0.001, num_attacks=100):

    train_dataset, test_dataset = load_har_data()
    print("Length of test_set: ", len(test_dataset[0]))
    dataset = train_dataset

    model = Treant()

    def single_attack(image, label):
        adversarial = attack_untargeted(model, dataset, image, label, alpha = alpha, beta = beta, iterations = 1000)
        print("True label: ", label)
        print("Predicted label: ", model.predict(image))
        print("Predicted label for adversarial example: ", model.predict(adversarial))
        return torch.norm(adversarial - image)

    print("\n\nRunning untargeted attack on {} random HAR instances for alpha= {} beta= {}\n\n".format(num_attacks, alpha, beta))
    total_distortion = 0.0

    #samples = [6312, 6891, 4243, 8377, 7962, 6635, 4970, 7809, 5867, 9559, 3579, 8269, 2282, 4618, 2290, 1554, 4105, 9862, 2408, 5082, 1619, 1209, 5410, 7736, 9172, 1650, 5181, 3351, 9053, 7816, 7254, 8542, 4268, 1021, 8990, 231, 1529, 6535, 19, 8087, 5459, 3997, 5329, 1032, 3131, 9299, 3910, 2335, 8897, 7340, 1495, 5244,8323, 8017, 1787, 4939, 9032, 4770, 2045, 8970, 5452, 8853, 3330, 9883, 8966, 9628, 4713, 7291, 9770, 6307, 5195, 9432, 3967, 4757, 3013, 3103, 3060, 541, 4261, 7808, 1132, 1472, 2134, 634, 1315, 8858, 6411, 8595, 4516, 8550, 3859, 3526]
    samples = list(range(num_attacks))
    #true_labels = [3, 1, 6, 6, 9, 2, 7, 5, 5, 3, 3, 4, 5, 6, 7, 9, 1, 6, 3, 4, 0, 6, 5, 9, 7, 0, 3, 1, 6, 6, 9, 6, 4, 7, 6, 3, 4, 3, 4, 3, 0, 7, 3, 5, 3, 9, 3, 1, 9, 1, 3, 0, 2, 9, 9, 2, 2, 3, 3, 3, 0, 5, 2, 5, 2, 7, 2, 2, 5, 7, 4, 9, 9, 0, 0, 7, 9, 4, 5, 5, 2, 3, 5, 9, 3, 0, 9, 0, 1, 2, 9, 9]
    for idx in samples:
        #idx = random.randint(100, len(test_dataset)-1)
        image, label = test_dataset[0][idx], test_dataset[1][idx]
        print("\n\n\n\n======== Instance %d =========" % idx)
        #target = None if not isTarget else random.choice(list(range(label)) + list(range(label+1, 10)))
        total_distortion += single_attack(image, label)
    
    print("Average distortion on random {} images is {}".format(num_attacks, total_distortion/num_attacks))

if __name__ == '__main__':
    timestart = time.time()
    random.seed(0)
    
    attack_mnist(sys.argv[1], int(sys.argv[2]), alpha=2, beta=0.005)
    #attack_har(alpha=5, beta=0.001, num_attacks=3)
    #attack_imagenet(arch='resnet50', alpha=10, beta=0.005, isTarget= False)
    #attack_imagenet(arch='vgg19', alpha=0.05, beta=0.001, isTarget= False, num_attacks= 10)

    #attack_mnist(alpha=2, beta=0.005, isTarget= True)
    #attack_cifar10(alpha=5, beta=0.001, isTarget= True)
    #attack_imagenet(arch='resnet50', alpha=10, beta=0.005, isTarget= True)
    #attack_imagenet(arch='vgg19', alpha=0.05, beta=0.001, isTarget= True, num_attacks= 10)

    timeend = time.time()
    print("\n\nTotal running time: %.4f seconds\n" % (timeend - timestart))

#!/usr/bin/env python3
import time
import random 
import numpy as np
import pandas as pd
import torch
import subprocess
#import torch.nn as nn

import torchvision.datasets as dsets
from torch.utils.data import DataLoader
import torchvision.transforms as transforms
# from torch.autograd import Variable
# from torch.utils.data.dataset import Dataset
# import os
# from PIL import Image
# #import pretrainedmodels
# #import pretrainedmodels.utils as utils
# import torchvision.models as models

# Hyperparameters
num_epochs = 50
batch_size = 128
learning_rate = 0.001



# class MNIST(nn.Module):
#     def __init__(self):
#         super(MNIST, self).__init__()
#         self.features = self._make_layers()
#         self.fc1 = nn.Linear(1024,200)
#         self.relu = nn.ReLU()
#         self.fc2 = nn.Linear(200,200)
#         self.dropout = nn.Dropout(p=0.5)
#         self.fc3 = nn.Linear(200,10)

#     def forward(self, x):
#         out = self.features(x)
#         out = out.view(out.size(0), -1)
#         out = self.fc1(out)
#         out = self.relu(out)
#         out = self.dropout(out)
#         out = self.fc2(out)
#         out = self.relu(out)
#         out = self.dropout(out)
#         out = self.fc3(out)
#         return out

#     def _make_layers(self):
#         layers=[]
#         in_channels= 1
#         layers += [nn.Conv2d(in_channels, 32, kernel_size=3),
#                    nn.BatchNorm2d(32),
#                    nn.ReLU()]
#         layers += [nn.Conv2d(32, 32, kernel_size=3),
#                    nn.BatchNorm2d(32),
#                    nn.ReLU()]
#         layers += [nn.MaxPool2d(kernel_size=2, stride=2)]
#         layers += [nn.Conv2d(32, 64, kernel_size=3),
#                    nn.BatchNorm2d(64),
#                    nn.ReLU()]
#         layers += [nn.Conv2d(64, 64, kernel_size=3),
#                    nn.BatchNorm2d(64),
#                    nn.ReLU()]
#         layers += [nn.MaxPool2d(kernel_size=2, stride=2)]
        
#         return nn.Sequential(*layers)


#     def predict(self, image):
#         self.eval()
#         image = torch.clamp(image,0,1)
#         image = Variable(image, volatile=True).view(1,1,28,28)
#         if torch.cuda.is_available():
#             image = image.cuda()
#         output = self(image)
#         _, predict = torch.max(output.data, 1)
#         return predict[0]

#     def predict_batch(self, image):
#         self.eval()
#         image = torch.clamp(image,0,1)
#         image = Variable(image, volatile=True)
#         if torch.cuda.is_available():
#             image = image.cuda()
#         output = self(image)
#         _, predict = torch.max(output.data, 1)
#         return predict



# class SimpleMNIST(nn.Module):
#     """ Custom CNN for MNIST
#         stride = 1, padding = 2
#         Layer 1: Conv2d 5x5x16, BatchNorm(16), ReLU, Max Pooling 2x2
#         Layer 2: Conv2d 5x5x32, BatchNorm(32), ReLU, Max Pooling 2x2
#         FC 10
#     """
#     def __init__(self):
#         super(SimpleMNIST, self).__init__()
#         self.layer1 = nn.Sequential(
#             nn.Conv2d(1, 16, kernel_size=5, padding=2),
#             nn.BatchNorm2d(16),
#             nn.ReLU(),
#             nn.MaxPool2d(2)
#             )
#         self.layer2 = nn.Sequential(
#             nn.Conv2d(16, 32, kernel_size=5, padding=2),
#             nn.BatchNorm2d(32),
#             nn.ReLU(),
#             nn.MaxPool2d(2)
#             )
#         self.fc = nn.Linear(7*7*32, 10)
        
#     def forward(self, x):
#         out = self.layer1(x)
#         out = self.layer2(out)
#         out = out.view(out.size(0), -1)
#         out = self.fc(out)
#         return out

#     def predict(self, image):
#         self.eval()
#         image = Variable(image.unsqueeze(0))
#         output = self(image)
#         _, predict = torch.max(output.data, 1)
#         return predict[0]

class MNIST():
    def __init__(self,model):
        self.cmdlst = ["mpirun", "-np", "1", "./treant", "--model", model, "--classify"]

    def predict(self, inst):
        inst=inst*255
        repr = str(inst)
        starting = repr.find("[[[")
        closing = repr.find("]]]")
        repr = repr[starting+3:closing].replace(' ', '').replace('\n','').replace(']','').replace('[','')
        pr = subprocess.run(self.cmdlst+[repr], text=True, capture_output=True)
        if pr.returncode != 0:
            raise RuntimeError("treant died: {}".format(pr.stderr))
        res = int(pr.stdout)
        return res

class Treant():
    def __init__(self):
        self.cmdlst = ["./treant", "--model", "data/har/models/Standard-D8.cereal", "--classify"]

    def predict(self, inst):
        repr = str(inst)
        closing = repr.find("]")
        repr = repr[9:closing].replace(' ', '').replace('\n','')
        pr = subprocess.run(self.cmdlst+[repr], text=True, capture_output=True)
        if pr.returncode != 0:
            raise RuntimeError("treant died: {}".format(pr.stderr))
        res = int(pr.stdout)
        return res
        #subprocess.call() 
        #return predict[0]

def show_image(img):
    """
    Show MNSIT digits in the console.
    """
    remap = "  .*#"+"#"*100
    img = (img.flatten()+.5)*3
    if len(img) != 784: return
    for i in range(28):
        print("".join([remap[int(round(x))] for x in img[i*28:i*28+28]]))

def load_har_data():
    train = pd.read_csv("data/har/train.csv.bz2")
    test = pd.read_csv("data/har/test.csv.bz2")
    trainX_np = train.iloc[:, :-1].values
    testX_np = test.iloc[:, :-1].values
    trainY_np = train.iloc[:, -1].values - 1
    testY_np = test.iloc[:, -1].values - 1
    trainX_torch = torch.from_numpy(trainX_np)
    testX_torch = torch.from_numpy(testX_np)
    trainY_torch = torch.from_numpy(trainY_np)
    testY_torch = torch.from_numpy(testY_np)
    return (trainX_torch, trainY_torch), (testX_torch, testY_torch)

def load_mnist_data():
    """ Load MNIST data from torchvision.datasets 
        input: None
        output: minibatches of train and test sets 
    """
    # MNIST Dataset
    train_dataset = dsets.MNIST(root='./data/mnist', train=True, transform=transforms.ToTensor(), download=True)
    test_dataset = dsets.MNIST(root='./data/mnist', train=False, transform=transforms.ToTensor(), download=True)

    # Data Loader (Input Pipeline)
    train_loader = DataLoader(dataset=train_dataset, batch_size=10, shuffle=False)
    test_loader = DataLoader(dataset=test_dataset, batch_size=10, shuffle=False)

    return train_loader, test_loader, train_dataset, test_dataset

def save_model(model, filename):
    """ Save the trained model """
    torch.save(model.state_dict(), filename)

def load_model(model, filename):
    """ Load the training model """
    model.load_state_dict(torch.load(filename))

if __name__ == '__main__':
    train_loader, test_loader, train_dataset, test_dataset = load_mnist_data()
#     net = MNIST()
#     if torch.cuda.is_available():
#         net.cuda()
#         net = torch.nn.DataParallel(net, device_ids=[0])
#         #net = torch.nn.DataParallel(net, device_ids=range(torch.cuda.device_count()))
#     train_mnist(net, train_loader)
#     #load_model(net, 'models/mnist_gpu.pt')
#     #load_model(net, 'models/mnist.pt')
#     test_mnist(net, test_loader)
#     #test_cifar10(net, test_loader)
#     save_model(net,'./models/mnist.pt')
#     #net.eval()


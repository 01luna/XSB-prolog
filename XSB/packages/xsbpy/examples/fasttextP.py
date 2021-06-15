
#written by Albert Ki

import fasttext as ft

def hello_world(binpath):
    sentence = ['Bonjour le monde']
    print(sentence[0])
    model = load_model(binpath)
    predictions = model.predict(sentence)
    lang = predictions[0][0][0]
    confidence = predictions[1][0][0]
    return(lang,confidence)

# Loads training model from the file 
def load_model(binpath):  
    print('Loading training model',binpath)
    model = ft.load_model(binpath)
    print(type(model))
    return(model)

# FastText.predict() as a python function
def predict2(binpath, sentences):
    model = ft.load_model(binpath)
    print(type(model))
    predictions = model.predict(sentences)
    lang = predictions[0][0][0]
    confidence = predictions[1][0][0]
    return((lang,confidence))

import codecs
import nltk

fp1 = codecs.open("/home/tj/big_data/data/jinyong/utf8/x1.txt", "r", "utf-8")

text = fp1.read()
text1 = text.split(' ')

fdist1 = nltk.FreqDist(text1)
fdist2 = sorted(fdist1.iteritems(), key=lambda d:d[1], reverse = True)

for i in xrange(0,200):
    print (fdist2[i][0] + ":" + str(fdist2[i][1]))

fp1.close()


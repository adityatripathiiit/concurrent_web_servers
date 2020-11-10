import random
import subprocess

NUMBER_OF_CONCURRENT_USERS = 100
MAX_LINE_COUNT = 10**4

def generate_big_random_sentences(filename):
    linecount = random.randint(1,MAX_LINE_COUNT)
    nouns = ("puppy", "car", "rabbit", "girl", "monkey")
    verbs = ("runs", "hits", "jumps", "drives", "barfs")
    adv = ("crazily.", "dutifully.", "foolishly.", "merrily.", "occasionally.")
    adj = ("adorable", "clueless", "dirty", "odd", "stupid")
    all = [nouns, verbs, adj, adv]
    with open(filename,'w') as f:
        for i in range(linecount):
            f.writelines([' '.join([random.choice(i) for i in all]),'\n'])
    f.close()
    pass


print("Generating files for Load ....")
for i in range(NUMBER_OF_CONCURRENT_USERS):
    generate_big_random_sentences( "./load/"+ str(i+1)+".html")


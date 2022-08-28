import vk
import sys

class vk_api:
   def __init__(self, vk_access_token):
       session = vk.Session(access_token=vk_access_token)
       vk_api = vk.API(session, v='5.102')  # подключаем вк

if __name__ == '__main__':
    print("dfsfd")
    for i in range(1, len(sys.argv)):
        print(sys.argv[i])
    # vka = vk_api("")

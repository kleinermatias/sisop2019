# sisop2019
TP2

```
docker volume create --driver local \
      --opt type=none \
      --opt device=/home/matias/Documentos/docker \
      --opt o=bind \
      sisop2019
docker pull ubuntu:bionic
```

```
docker run -it --name sisop -v sisop2019:/root ubuntu:bionic
```

sudo docker run -d --rm --network handson4 --name carts-db -e DNS=carts-db gusabary/carts-db:v3
sudo docker run -d --rm --network handson4 --name user-db -e DNS=user-db gusabary/user-db:v3
sudo docker run -d --rm --network handson4 --name orders-db -e DNS=orders-db gusabary/orders-db:v4
sudo docker run -d --rm --network handson4 --name catalog-db -e DNS=catalogue-db gusabary/catalog-db:v4
sudo docker run -d --rm --network handson4 --name catalog -e DNS=catalogue gusabary/catalog:v33
sudo docker run -d --rm --network handson4 --name user -e DNS=user -e MONGO_HOST=user-db:27017 gusabary/user:v2
sudo docker run -d --rm --network handson4 --name cart -e DNS=carts gusabary/cart:v17
sudo docker run -d --rm --network handson4 --name shipping -e DNS=shipping gusabary/shipping:v3
sudo docker run -d --rm --network handson4 --name payment -e DNS=payment gusabary/payment:v2
sudo docker run -d --rm --network handson4 --name orders -e DNS=orders gusabary/orders:v3
sudo docker run -d --rm --network handson4 --privileged --name frontend -p 8079:8079 -e DNS=frontend gusabary/frontend:v5

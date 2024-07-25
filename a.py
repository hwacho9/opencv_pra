import numpy as np
from PIL import Image
from sklearn.cluster import KMeans


def posterize_kmeans(image, n_colors):
    img_array = np.array(image)
    h, w, d = img_array.shape
    img_array = img_array.reshape(-1, d)

    kmeans = KMeans(n_clusters=n_colors, random_state=0).fit(img_array)
    new_colors = kmeans.cluster_centers_[kmeans.predict(img_array)]

    new_image = new_colors.reshape(h, w, d)
    new_image = np.uint8(new_image)
    return Image.fromarray(new_image)


# 画像をポスタリゼーション
n_colors = 4
image_path = "/Users/chosunghwa/Desktop/workspace/opencv_pra/cat.jpg"
image = Image.open(image_path)
posterized_image = posterize_kmeans(image, n_colors)
posterized_image.show()

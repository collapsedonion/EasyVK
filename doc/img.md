# Image
Holds vk::Image and all data describing it, automatically allocates image, create only with [EasyVK::Device::createImage](ldevice.md#devicecreateimage), **do not use the constructor**.

## Image::bind()
Binds allocated memory to host and returns pointer, only works if created with Host accessibility flag.

```c++
void* bind();
```

## Image::unbind()
Unbinds memory from host.

```c++
void unbind();
```

## Image::getView()
Creates and returns view of this Image.

```c++
View getView();
```
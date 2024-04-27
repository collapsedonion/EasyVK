# Buffer
Buffer object handles allocated buffer, the only valid way to create it is [EasyVK::Device::createBuffer](ldevice.md#devicecreatebuffer), **do not use the constructor**.

## Buffer::bind()
Available only if upon creation Host accessibility was turned on, return pointer to buffer start in memory.

```c++
void* bind();
```

## Buffer::unbind()
Unbinds buffer if it wat bound.

```c++
void unbind();
```
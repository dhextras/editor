# implementations to do

- [x] Stream input in rawmode & out put it
- [x] Print out put properly and handle cursor movment, tab, enter, backspace
- [x] create a temp buffer and edit and do stuff with it then print the edited version in the end
- [x] Gap buffer implementation for each lines
- [x] fix reallocating more memory for the gap in the middle
- [x] Split the file into modules
- [x] Update the Buffer to be consist of arrays of buffers each having their own gap buffer implementation
- [x] Implement moving up and down functions
- [ ] open a file and read it's content
- [ ] Use the file content instead of the predefined string to a file & then saving it
- [ ] Final touch up should be bringing the customization on the first time the editor even got opened they can skip and make configuration on their own later with using plugins and shit, show them whats happening under the hood like creating files and shits all those shits shit should be shown in the start

## compile

```bash
make
```

## run

```bash
build/editor
```

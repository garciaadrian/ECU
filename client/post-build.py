# This file is ran by webpack AFTER compilation
# It will create result.html in the bin folder
# with the contents of index.html and the bundle.js file

with open('index.html') as index:
    index_content = index.read()

with open('../build/bin/bundle.js') as bundle:
    bundle_content = bundle.read()

with open('../build/bin/result.html', 'w') as result:
    result.write(index_content)
    result.write("<script>")
    result.write(bundle_content)
    result.write("</script>")

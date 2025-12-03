// prejs.js

var Module = {
    
    print :(text) => {
        console.log("stdout: " + text);
    },
    printErr :(text) => {
        alert("stderr: " + text);
    },
    onRuntimeInitialized : () => {
        
    }
  }
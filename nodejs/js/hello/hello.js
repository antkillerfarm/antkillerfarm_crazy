var person = {
    name: 'Bob',
    age: 20,
    tags: ['js', 'web', 'mobile'],
    city: 'Beijing',
    hasCar: true,
    zipcode: null
};

class Student {
    constructor(name) {
        this.name = name;
    }

    hello() {
        console.log(this.name);
    }
}

//alert(person.name);
aa.innerHTML = person.name + ":" + person.age;
document.getElementById('bb').innerHTML = "Jack Ma";
var node = $('#cc').html('Jack Ma Ma Ma');

var xiaoming = new Student('小明');
xiaoming.hello();
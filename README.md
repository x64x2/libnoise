libnoise is a portable C++ library that is used to generate coherent noise, a type of smoothly-changing noise. libnoise can generate perlin noise, ridged multifractal noise, and other types of coherent noise.

coherent noise is often used by graphics programmers to generate natural-looking textures, planetary terrain, and other things. It can also be used to move critters in a realistic way. libnoise is known to compile using the following compiler:

 - gcc 9.4.0 under Gentoo Linux(x86)

it is not known if libnoise will compile on 64-bit platforms like winducks.


 ## noise modules

in libnoise, coherent-noise generators are encapsulated in classes called noise modules, there are many different types of noise modules. some noise modules can combine or modify the outputs of other noise modules in various ways; you can join these modules together to generate very complex coherent noise.

a noise module receives a 3-dimensional input value from the application, computes the noise value given that input value, and returns the resulting value back to the application.

if the application passes the same input value to a noise module, the noise module returns the same output value.


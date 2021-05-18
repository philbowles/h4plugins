![NODE PINK](../assets/nodepink.jpg)
![H4P Flyer](../assets/GPIOLogo.jpg)

# GPIO Handling and "NODE-PINK": the logical approach

# Introduction

Let's face it: there are only so many things you can do with a GPIO pin. Yes, there are thousands of different type of sensor out there but each and every one of them can only set a GPIO pin HIGH (usually 5v or 3.3v) or LOW (usually 0v) or put an analog voltage on it. What differentiates them and makes them clever is *the way* they do that.

Unfortunately, "clever" in the real world often translates to "difficult" because even  the most expensive of switches will have "switch bounce". If you don't know what that is, here's an [excellent article](http://www.ganssle.com/debouncing.htm) explaining it all and how to deal with it. Many devices you might want to hook up are bouncy, noisy, or complicated. Some e.g. rotary encoders are usually all three at once...

[H4P_PinMachine](h4gm.md) provides easy-to-use "strategies" for many of the common behaviours of buttons, switches and sensors. It does all the "heavy lifting", usually in a single line of code and simply calls you when an interesting event occurs on the GPIO pin.

---

# Logical vs Physical

Hopefully, you will already be familiar with Arduino's `digitalRead` and `digitalWrite` as the "standard" way of dealing with GPIO pins. To use them successfully you need to understand the concept of "Active High" and "Active Low".

Most of us think of a sensor or LED for example as being "ON" when there is a voltage on the GPIO pin after we set it high - usually Vcc of 3.3v or 5v depending on the board /arhcitecture: `digitalWrite(MY_PIN,HIGH);` and this is what is known as "Active High". We say that the device is "active" (or "ON") when the GPIO pin is HIGH, which is all quite easy to understand.

But many devices work "the other way round": they are "ON" when the GPIO pin is LOW and OFF when it is high and that can get confusing. Most ESP8266 boards have the built-in LED wired this way. Many that have a user button (e.g. nodeMCU) also have the button wired as "Active Low".

Dealing with GPIO this way is the *physical* way of doing it, we have to write the actual 1 or 0 to the GPIO pin.

[H4P_PinMachine](h4gm.md) and NODE_PINK work differently. When you attach a flow to a pin, you tell it whether the pin is active high or low. From that point onwards you set it ON or OFF to correspond with the real-world concept of whether it is actually "doing its thing" or not. This is best shown in an example, using [H4P_PinMachine](h4gm.md)'s `logicalWrite` function:

```cpp
// We declare out pin is "Active Low" (ON when pin=0) and set it intially OFF
h4gmOutput builtin(LED_BUILTIN,ACTIVE_LOW,OFF);
...
builtin.logicalWrite(LED_BUILTIN,ON); // does exactly what it says on the tin: turns the LED ON

```

If we were dealing with a more conventional "active high" device, the code would look like this:


```cpp
// We declare out pin is "Active High" (ON when pin=1) and set it intially OFF
h4gmOutput builtin(LED_BUILTIN,ACTIVE_HIGH,OFF);
...
builtin.logicalWrite(LED_BUILTIN,ON); // does exactly what it says on the tin: turns the LED ON
```

Note that while the declaration is - necessarily - different, the rest of the code is exacty the same: ON always means the kind of ON we normally like it to mean! All of which brings us to...

## Benefits of the logical approach

The code above sets the `pinMode` for you and also sets the initial state of the pin to its logical state (in this case, OFF). Already you have saved a little code, even if its only one line but the benefits continue:

* If you move the code to a different board whose LED works the opposite way, all you change is `ACTIVE_LOW` to `ACTIVE_HIGH` and all the rest of your code will work without needing to find every `digitalWrite` and change HIGH to LOW.
  
* It's mentally easier: `logicalWrite(pin,ON)` always turns the LED ON, whether its active low or high. Get it right once at the start and never worry about it again.
  
* *IF* you get it wrong first time, one tiny change in the declaration is all it takes to fix everything else.

---

# Essential further reading:

* [GPIO/NODE-PINK: basic flows](basic.md)
* [GPIO/NODE-PINK: rotary encoder flows](encoders.md)
* :building_construction: [GPIO/NODE-PINK: analog flows](analog.md)
* :building_construction: [GPIO/NODE-PINK: advanced techniques](nodepinkadv.md)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.c
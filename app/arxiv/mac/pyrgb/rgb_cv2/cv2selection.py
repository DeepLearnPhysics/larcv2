import abc
from .. import QtGui


class CV2Selection(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        self.name = "CV2Selection"
        self.options = {}
        self.types = {}
        self.widgets = {}

    def description(self):
        return self.__description__()

    def set_options(self, options):
        for op in options:
            assert op in self.options.keys()
            self.options[op] = options[op]

    def apply(self, image):
        return self.__implement__(image)

    def dump(self):
        print "\t name: {}".format(self.name)
        print "\t options: {}".format(self.options)
        print "\t types: {}".format(self.types)

    @abc.abstractmethod
    def __description__(self):
        """ provide description for tooltip """

    @abc.abstractmethod
    def __parsewidgets__(self):
        """ implement how to parse widgets """

    @abc.abstractmethod
    def __implement__(self, image):
        """ implement cv2 function """

    def parsewidgets(self):
        self.__parsewidgets__()

    def data2str(self, _type, _data):

        if _type is tuple:
            text = "("
            for d in _data: text += str(o) + ","
            text = text[:-1] + ")"
            return text

        if _type is int:
            return str(_data)

        if _type is float:
            return str(_data)

        raise Exception("Please implement type: {} in data2str cv2selection".format(type(_data)))

    def str2data(self, _type,_str):

        if _type is tuple:
            t = _str[1:-1].split(",")
            return tuple([int(i) for i in t])

        if _type is int:
            return int(_str)

        if _type is float:
            return float(_str)

        if _type is list:
            t = _str[1:-1].split(",")
            return [float(i) for i in t]

        raise Exception("Please implement type: {} in str2data cv2selection".format(type(_data)))

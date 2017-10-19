from cv2selection import CV2Selection


class CV2Nothing(CV2Selection):

    def __init__(self):
        super(CV2Nothing, self).__init__()

        self.name = "CV2Nothing"

    def __description__(self):
        return "No description provided!"
    
    def __parsewidgets__(self):
        print "Nothing to parse"

    def __implement__(self, image):
        return image

from setuptools import setup

setup(name='worklight',
      version='0.1',
      description='A hardware light that will react on certain calendar events',
      url='http://github.com/lenalebt/worklight',
      author='Lena Brüder',
      author_email='oss@lena-brueder.de',
      license='MIT',
      packages=['worklight'],
      install_requires=[
        'icalendar',
        'configparser',
        'pyserial',
      ],
      zip_safe=False)

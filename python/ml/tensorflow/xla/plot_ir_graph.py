# pip install graphviz
from graphviz import Digraph

def plot_from_entry(path:str, fig_name:str, Lstart:int, Lend:int):
    '''
    Lstart: line start from "ENTRY"
    Lend:   line end with "}"
    '''
    data = None
    with open(path) as f:
        data = f.readlines()
        data = data[Lstart :Lend - 2]
    
    fig = Digraph(comment='The IR graph')
    for line in data:
        line = line.split(' ')
        node_name = ''
        node_shape = ''
        node_dtype = ''
        inputs = []
        for i, word in enumerate(line):
            if word != '' and word[0] == '%':
                if node_name == '':
                    node_name = word[1:]
                    shape_start = line[i+2].find('[')
                    node_shape = line[i+2] \
                            [shape_start:line[i+2].rfind(']') + 1]
                    node_dtype = line[i+2] \
                            [shape_start - 3:shape_start]
                else:
                    inputs.append(word[1:].rstrip('\n').rstrip(',').rstrip(')'))
        fig.node(node_name, node_name + node_dtype + node_shape, shape='box')
        for input in inputs:
            fig.edge(input, node_name)
    
    fig.render(fig_name, format='jpg', view=False, cleanup=True)

if __name__ == "__main__":
    log = 'a.log'
    plot_from_entry(path=log, fig_name='IR', Lstart=41103, Lend=41112)
